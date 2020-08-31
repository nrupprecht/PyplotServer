#include "PyplotClient.h"

namespace NatPlotLib {

  void PyplotClient::clear_buffers() {
    client_socket.send(MessageType::ClearBuffers);
  }

  void PyplotClient::clear_fig() {
    client_socket.send(MessageType::ClearFig);
  }

  void PyplotClient::plot(const sig x_buffer, const sig y_buffer) {
    client_socket.send(MessageType::Plot);
    client_socket.send(x_buffer);
    client_socket.send(y_buffer);
    client_socket.send(sig(0));
  }

   void PyplotClient::plot(const sig x_buffer, const sig y_buffer, const std::string& label) {
    client_socket.send(MessageType::Plot);
    client_socket.send(x_buffer);
    client_socket.send(y_buffer);
    client_socket.send(sig(1));
    client_socket.send(label);
  }
		      
  void PyplotClient::scatter(const sig x_buffer, const sig y_buffer) {
    client_socket.send(MessageType::Scatter);
    client_socket.send(x_buffer);
    client_socket.send(y_buffer);
    client_socket.send(sig(0));
  }

  void PyplotClient::scatter(const sig x_buffer, const sig y_buffer, const std::string& label) {
    client_socket.send(MessageType::Scatter);
    client_socket.send(x_buffer);
    client_socket.send(y_buffer);
    client_socket.send(sig(1));
    client_socket.send(label);
  }

  void PyplotClient::save_fig(const std::string& fname) {
    client_socket.send(MessageType::SaveFig);
    client_socket.send(static_cast<sig>(fname.size()));
    client_socket.send(&fname[0], fname.size());    
  }

  void PyplotClient::show_fig() {
    client_socket.send(MessageType::ShowFig);
  }

  void PyplotClient::make_fig(const double w, const double h) {
    client_socket.send(MessageType::MakeFig);
    client_socket.send(w);
    client_socket.send(h);
  }

  void PyplotClient::reset_opt() {
    client_socket.send(MessageType::ResetOpt);
  }

  void PyplotClient::xlim(const double xmin, const double xmax) {
    client_socket.send(xmin);
    client_socket.send(xmax);
  }

  void PyplotClient::ylim(const double ymin, const double ymax) {
    client_socket.send(ymin);
    client_socket.send(ymax);
  }

  void PyplotClient::legend() {
    client_socket.send(MessageType::Legend);
  }

  void PyplotClient::axis_labels(const std::string& xlbl, const std::string& ylbl) {
    client_socket.send(MessageType::AxisLabels);
    client_socket.send(xlbl);
    client_socket.send(ylbl);
  }

  void PyplotClient::title(const std::string& title) {
    client_socket.send(MessageType::Title);
    client_socket.send(title);
  }

  void PyplotClient::exit() {
    client_socket.send(MessageType::Exit);
  }

  void PyplotClient::new_figure(const double w, const double h) {
    clear_fig();
    clear_buffers();
    reset_opt();
    make_fig(w, h);
  }

  void PyplotClient::store_x(const std::vector<double>& xvals) {
    store_buffer(1, xvals);
  }

  void PyplotClient::store_y(const std::vector<double>& yvals) {
    store_buffer(2, yvals);
  }

  void PyplotClient::plot_xy(const std::string& label) {
    if (label.empty()) plot(1, 2);
    else plot(1, 2, label);
  }

  void PyplotClient::scatter_xy(const std::string& label) {
    if (label.empty()) scatter(1, 2);
    else scatter(1, 2, label);
  }
  
}
