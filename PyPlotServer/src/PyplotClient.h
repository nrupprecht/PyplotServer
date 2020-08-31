#ifndef __PY_CLIENT_H__
#define __PY_CLIENT_H__

#include "Socket.h"

#include <vector>

// For fixed sized data types.
#include <cstdint>

namespace NatPlotLib {

  //! \brief The integer signal type.
  typedef uint64_t sig; 


  enum class MessageType : sig {
    ClearBuffers = 1, // Clears all buffers.
    ClearFig = 2, // Close figure
    Store = 3, // Store a value in a buffer. [ Buffer# | DataType | DataLength | Data ]
    Plot = 4, // [ X buffer# | Y buffer# | Linestyle buffer# | Color buffer# ]
    Scatter = 5, // [ X buffer# | Y buffer# | Linestyle buffer# | Color buffer# ]
    SaveFig = 6, // Save the current figure. [ File name buffer# ]
    ShowFig = 7, // Show the current figure.
    MakeFig = 8,
    ResetOpt = 9,
    AddOpt = 10,
    XLim = 11, // [ X min | X max ]
    YLim = 12, // [ Y min | Y max ]
    Legend = 13,
    AxisLabels, // [ X label | Y label ]
    Title,
    Exit,
  };


  enum class DataType : sig {
    Double = 1,
    String = 2,
    Array = 3,
  };

  
  class PyplotClient {
  public:
    PyplotClient(int port) : client_socket(port) {};

    bool initialize() {
      return client_socket.connect();
    }

    // --- Atomic instructions ---
    
    void clear_buffers();
    void clear_fig();
    
    template<typename T>
    void store_buffer(sig buffer_number, const T& data) {
      client_socket.send(MessageType::Store);
      client_socket.send(buffer_number);
      send_data(data);
    }

    void plot(const sig, const sig);
    void plot(const sig, const sig, const std::string&);
    void scatter(const sig, const sig);
    void scatter(const sig, const sig, const std::string&);
    void save_fig(const std::string&);
    void show_fig();
    void make_fig(const double=0, const double=0);
    void reset_opt();

    template<typename T>
    void add_opt(const std::string& key, const T& value) {
      client_socket.send(MessageType::AddOpt);
      // The server expects a string as the key, so we can just send the string.
      client_socket.send(static_cast<sig>(key.size()));
      client_socket.send(&key[0], key.size());
      // We can send any kind of data for the value, so we have to use the full function.
      send_data(value);
    }

    void xlim(const double, const double);
    void ylim(const double, const double);
    void legend();
    void axis_labels(const std::string&, const std::string&);
    void title(const std::string&);

    // --- Complex instructions ---

    void new_figure(const double=0, const double=0);
    void store_x(const std::vector<double>&);
    void store_y(const std::vector<double>&);
    void plot_xy(const std::string& = "");
    void scatter_xy(const std::string& = "");
    void exit();
    
  private:
    // Data serialization

    template<typename T> void send_data(const T&);

    template<size_t N>
    void send_data(const char (&cstr) [N]) {
      client_socket.send(DataType::String);
      client_socket.send(static_cast<sig>(N - 1));
      // Size is N - 1 to eliminate the trailing \0.
      client_socket.send(cstr, N - 1);
    }
    
    template<>
    void send_data(const std::string& str) {
      client_socket.send(DataType::String);
      client_socket.send(static_cast<sig>(str.size()));
      client_socket.send(&str[0], str.size());
    }

    template<>
    void send_data(const std::vector<double>& data) {
      client_socket.send(DataType::Array);
      client_socket.send(static_cast<sig>(data.size()));
      client_socket.send(&data[0], data.size());
    }

    template<>
    void send_data(const double& value) {
      client_socket.send(DataType::Double);
      client_socket.send(&value, 1);
    }
    
  private:
    Socket client_socket;
  };
  
}
#endif // __PY_CLIENT_H__
