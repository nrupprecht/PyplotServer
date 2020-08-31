#include "../PyplotClient.h"
using namespace NatPlotLib;

#include <algorithm>
#include <cmath>

int main(int argc, char const *argv[]) {
  int comm_port = 8080;

  PyplotClient client(comm_port);
  client.initialize();

  
  client.clear_buffers();
  client.clear_fig();

  std::vector<double> xvals(100);
  double val(-0.01);
  
  std::generate(xvals.begin(), xvals.end(), [&]() { return val += 0.1; });
  std::vector<double> yvals(100);
  val = -0.01;
  std::generate(yvals.begin(), yvals.end(), [&]() { return sin(val += 0.1); });

  client.make_fig(8., 6.);
  client.store_buffer(1, xvals);
  client.store_buffer(2, yvals);
  client.add_opt("c", "black");
  client.add_opt("linestyle", "--");
  client.plot(1, 2, "Data");
  client.reset_opt();
  client.add_opt("c", "blue");
  client.scatter(1, 2, "Points");
  client.legend();
  client.title("My Figure!");
  client.axis_labels("", "Y axis?");
  client.save_fig("/Users/nathaniel/fig.png"); // length = 24
  client.exit();

  return 0; 
} 
