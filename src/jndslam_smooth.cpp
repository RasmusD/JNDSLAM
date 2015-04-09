/* Copyright 2015 Rasmus Dall - rasmus@dall.dk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include "jndslam_smooth.h"

// Smoothing using LOWESS
// This is closely based on the following C# implementation http://stackoverflow.com/questions/5651709/c-sharp-loess-lowess-regression
// But adapted to current use. This also explains the insistence on the use of doubles over floats which isn't really necessary.
void smooth(std::vector<std::vector<float> > &pitchs, float smoothing_span, unsigned int iters, bool ignore_unvoiced)
{
  // Check if smoothing_span is valid
  if (smoothing_span < 0 || smoothing_span > 1)
  {
    throw std::invalid_argument("Smoothing span must be between 0 and 1 I got "+std::to_string(smoothing_span));
  }
  // Create x and y vectors
  std::vector<double> x_vals;
  std::vector<double> y_vals;
  // Counter for number of voiced segments, only used if ignoring unvoiced segments.
  int v_count = 0;
  for (int i = 0; i < pitchs.size(); i++)
  {
    // If we ignore unvoiced segments each X is evenly spaced
    if (ignore_unvoiced && pitchs[i][1] == 1)
    {
      // X-value
      x_vals.push_back((double)v_count);
      v_count++;
      // Y-value
      y_vals.push_back((double)pitchs[i][2]);
    }
    else if (pitchs[i][1] == 1) // If we don't ignore them we use the x in ms
    {
      // X-value
      x_vals.push_back((double)pitchs[i][0]);
      // Y-value
      y_vals.push_back((double)pitchs[i][2]);
    }
  }
  // The number of datapoints
  const int n = x_vals.size();
  // If we have 0 datapoints this is unvoiced and we simply clear pitchs and return (stylise will pick up on this)
  if (n == 0)
  {
    pitchs.clear();
    return void();
  }
  else if (n == 1) // We can return early if n is 1 or 2 - In fact we can currently just clear pitchs atm but later this is better.
  {
    std::vector<std::vector<float> > new_pitch;
    new_pitch.push_back({(float)pitchs[x_vals[0]][0], 1, (float)y_vals[0]});
    pitchs.swap(new_pitch);
    return void();
  }
  else if (n == 2)
  {
    std::vector<std::vector<float> > new_pitch;
    new_pitch.push_back({(float)pitchs[x_vals[0]][0], 1, (float)y_vals[0]});
    new_pitch.push_back({(float)pitchs[x_vals[1]][0], 1, (float)y_vals[1]});
    pitchs.swap(new_pitch);
    return void();
  }
  // The smoothing window as determined by the smoothing span
  int smoothing_window = ceil((float)n * smoothing_span);
  // We need a window of at least two points
  // Other implementations ignore this or throw an exception.
  // We simply set it to two in this case.
  if (smoothing_window < 2)
  {
    smoothing_window = 2;
  }
  
  // Parameters
  double result [n];
  double residuals [n];
  double robustness_weights [n];
  
  // Set all weights to 1.
  for (int i = 0; i < n; i++)
  {
    robustness_weights[i] = 1;
  }
  // Do [iters] + 1 fits. I.e. we do an initial fit with all weights at 1 then [iter] additional fits.
  for (int i = 0; i <= iters; i++)
  {
    int smoothing_interval [2] = {0, smoothing_window - 1};
    // At each x, compute a local weighted linear regression
    for (int j = 0; j < n; j++)
    {
      double x = x_vals[j];
      
      // Find out the interval of source points on which
      // a regression is to be made.
      if (j > 0)
      {
        // the implementation this is based on forgets to check left side.
        int left = smoothing_interval[0];
        int right = smoothing_interval[1];
        // The edges should be adjusted if the previous point to the left
        // is closer to x than the current point to the right
        if (left != 0 && x - x_vals[left - 1] < x_vals[right] - x)
        {
          smoothing_interval[0]++;
          smoothing_interval[1]++;
        }
        // The edges should be adjusted if the next point to the right
        // is closer to x than the leftmost point of the current interval
        else if (right < n - 1 && x_vals[right + 1] - x < x - x_vals[left])
        {
          smoothing_interval[1]++;
          smoothing_interval[0]++;
        }
      }
      
      // Find the point in smoothing_interval farthest from x
      int left = smoothing_interval[0];
      int right = smoothing_interval[1];
      int edge;
      if (x - x_vals[left] > x_vals[right] - x)
      {
        edge = left;
      }
      else
      {
        edge = right;
      }
      
      // Compute a least-squares linear fit weighted by
      // the product of robustness weights and the tricube
      // weight function.
      // See http://en.wikipedia.org/wiki/Linear_regression
      // (section "Univariate linear case")
      // and http://en.wikipedia.org/wiki/Weighted_least_squares
      // (section "Weighted least squares")
      double sum_weights = 0;
      double sum_x = 0, sum_x_squared = 0, sum_y = 0, sum_xy = 0;
      double denom = abs(1.0 / (x_vals[edge] - x));
      for (int k = left; k <= right; ++k)
      {
        double xk = x_vals[k];
        double yk = y_vals[k];
        double dist;
        if (k < j)
        {
            dist = (x - xk);
        }
        else
        {
            dist = (xk - x);
        }
        double w = tricube(dist * denom) * robustness_weights[k];
        double xkw = xk * w;
        sum_weights += w;
        sum_x += xkw;
        sum_x_squared += xk * xkw;
        sum_y += yk * w;
        sum_xy += yk * xkw;
      }
      
      double mean_x = sum_x / sum_weights;
      double mean_y = sum_y / sum_weights;
      double mean_xy = sum_xy / sum_weights;
      double mean_x_squared = sum_x_squared / sum_weights;
      double beta;
      if (mean_x_squared == mean_x * mean_x)
      {
          beta = 0;
      }
      else
      {
          beta = (mean_xy - mean_x * mean_y) / (mean_x_squared - mean_x * mean_x);
      }
      
      double alpha = mean_y - beta * mean_x;
      
      result[j] = beta * x + alpha;
      residuals[j] = abs(y_vals[j] - result[j]);
    }
    
    // No need to recompute the robustness weights at the last
    // iteration, they won't be needed anymore
    if (i == iters)
    {
        break;
    }

    // Recompute the robustness weights.
    // Find the median residual.
    std::vector<double> sorted_residuals;
    for (int z = 0; z < n; z++)
    {
      sorted_residuals.push_back(residuals[z]);
    }
    std::sort(sorted_residuals.begin(), sorted_residuals.end());
    double median_residual = sorted_residuals[n / 2];

    if (median_residual == 0)
    {
        break;
    }

    for (int z = 0; z < n; z++)
    {
      double arg = residuals[i] / (6 * median_residual);
      robustness_weights[i] = (arg >= 1) ? 0 : pow(1 - arg * arg, 2);
    }
  }
  
  std::vector<std::vector<float> > new_pitch;
  // Create a new pitchs with the calculated curve
  for (int i = 0; i < n; i++)
  {
    std::cout << result[i] << std::endl;
    new_pitch.push_back({(float)pitchs[x_vals[i]][0], 1, (float)result[i]});
  }
  pitchs.swap(new_pitch);
}

// Smooth all sylls in an utt
void smooth_utt(typename utterance::utterance &utt)
{
  std::cout << utt.name << std::endl;
  for (int i = 0; i < utt.sylls.size(); i++)
  {
    std::cout << "Syll " << utt.sylls[i].identity << " " << i << std::endl;
    smooth(utt.sylls[i].pitch_values);
  }
}

// Smooth all sylls in all utts in a list of utts
void smooth_utts(std::vector<typename utterance::utterance> &utts)
{
  for (int i = 0; i < utts.size(); i++)
  {
    smooth_utt(utts[i]);
  }
}

// Compute the tricube
// weight function
// http://en.wikipedia.org/wiki/Local_regression#Weight_function
static double tricube(double x)
{
  double tmp = 1 - abs(x) * abs(x) * abs(x);
  return tmp * tmp * tmp;
}
