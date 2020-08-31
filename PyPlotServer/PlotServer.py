# Useful base: https://realpython.com/python-sockets/

import socket
import enum
import struct
import time

import numpy as np
import matplotlib.pyplot as plt

# Turn off interactive mode, so figures are only displayed when we explicitly say so.
plt.ioff()


class MessageType(enum.IntEnum):
    ClearBuffers = 1  # Clears all buffers.
    ClearFig = 2  # Close figure
    Store = 3  # Store a value in a buffer. [ Buffer  # | DataType | DataLength | Data ]
    Plot = 4  # [ X buffer  # | Y buffer # ]
    Scatter = 5  # [X buffer  # | Y buffer # ]
    SaveFig = 6
    ShowFig = 7
    MakeFig = 8  # [ Fig width | Fig height ]
    ResetOpt = 9  # []
    AddOpt = 10  # [ Key String | Data ]
    XLim = 11  # [ X min | X max ]
    YLim = 12  # [ Y min | Y max ]
    Legend = 13  # []
    AxisLabels = 14  # [ X label | Y label ]
    Title = 15  # [ Title ]
    Exit = 16


class DataType(enum.IntEnum):
    Double = 1,
    String = 2,
    Array = 3


class PlotServer:
    def __init__(self, host: str = "127.0.0.1", port: int = 8080):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = host
        self.port = port

        self.conn = None
        self.addr = None

        self.buffer = {}
        self.options = {}

        self._int_width = 8
        self._array_width = 8

    def send(self, message: str):
        send_back = str.encode(message)
        self.conn.sendall(send_back)

    def recv_int(self):
        return int.from_bytes(self.conn.recv(self._int_width), byteorder="little")

    def recv_double(self):
        return struct.unpack("d", self.conn.recv(8))[0]

    def recv_string(self):
        size = self.recv_int()
        buffer = self.conn.recv(size)
        return buffer.decode("utf-8")

    def recv_vector(self):
        size = self.recv_int()
        buffer = self.conn.recv(self._array_width * size)
        return struct.unpack(f"{size}d", buffer)

    def recv_data(self):
        data_type = DataType(self.recv_int())
        if data_type == DataType.Double:
            return self.recv_double()
        elif data_type == DataType.String:
            return self.recv_string()
        elif data_type == DataType.Array:
            return self.recv_vector()

    def run(self):
        self.socket.bind((self.host, self.port))
        self.socket.listen()
        self.conn, self.addr = self.socket.accept()
        with self.conn:
            ip, port = self.addr
            print(f"Connected by IP {ip} on port {port}.")

            while True:
                # Sleep a little, so the program doesn't take up too many resources.
                time.sleep(0.05)

                # Receive the type of message that is coming.
                message_type = self.recv_int()

                # Check if a message was received.
                if message_type == 0:
                    continue

                # A message was received.
                print(f"Message {MessageType(message_type).name}.")

                if message_type == MessageType.ClearBuffers:
                    self.buffer = {}
                elif message_type == MessageType.ClearFig:
                    plt.close("all")
                elif message_type == MessageType.MakeFig:
                    fig_width = self.recv_double()
                    fig_height = self.recv_double()
                    if fig_width != 0 and fig_height != 0:
                        plt.figure(figsize=(fig_width, fig_height))
                    else:
                        plt.figure()
                elif message_type == MessageType.Store:
                    buffer_number = self.recv_int()
                    data = self.recv_data()
                    if 0 < buffer_number:
                        self.buffer[buffer_number] = data
                elif message_type == MessageType.Plot:
                    xvals_buffer = self.recv_int()
                    yvals_buffer = self.recv_int()
                    has_label = self.recv_int()
                    if has_label == 1:
                        label = self.recv_string()
                        plt.plot(self.buffer[xvals_buffer], self.buffer[yvals_buffer], label=label, **self.options)
                    else:
                        plt.plot(self.buffer[xvals_buffer], self.buffer[yvals_buffer], **self.options)
                elif message_type == MessageType.Scatter:
                    xvals_buffer = self.recv_int()
                    yvals_buffer = self.recv_int()
                    has_label = self.recv_int()
                    if has_label:
                        label = self.recv_string()
                        plt.scatter(self.buffer[xvals_buffer], self.buffer[yvals_buffer], label=label, **self.options)
                    else:
                        plt.scatter(self.buffer[xvals_buffer], self.buffer[yvals_buffer], **self.options)
                elif message_type == MessageType.SaveFig:
                    fname = self.recv_string()
                    plt.savefig(fname)
                elif message_type == MessageType.ShowFig:
                    plt.show()
                elif message_type == MessageType.ResetOpt:
                    self.options = {}
                elif message_type == MessageType.AddOpt:
                    key = self.recv_string()
                    value = self.recv_data()
                    self.options[key] = value
                elif message_type == MessageType.XLim:
                    xmin = self.recv_double()
                    xmax = self.recv_double()
                    plt.xlim([xmin, xmax])
                elif message_type == MessageType.YLim:
                    ymin = self.recv_double()
                    ymax = self.recv_double()
                    plt.xlim([ymin, ymax])
                elif message_type == MessageType.Legend:
                    plt.legend()
                elif message_type == MessageType.AxisLabels:
                    xlabel = self.recv_string()
                    ylabel = self.recv_string()
                    if xlabel != "":
                        plt.xlabel(xlabel)
                    if ylabel != "":
                        plt.ylabel(ylabel)
                elif message_type == MessageType.Title:
                    title = self.recv_string()
                    plt.title(title)
                elif message_type == MessageType.Exit:
                    break
