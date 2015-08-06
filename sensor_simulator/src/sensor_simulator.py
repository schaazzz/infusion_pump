"""
File    - sensor_simulator.py
Author  - Shahzeb Ihsan

Simulates sensor data transfer over the serial port or UDP.

Usage:
python sensor_simulator.py udp <Host IP> <Port>
OR
python sensor_simulator.py com <Com Port 1..N> <Baudrate>
"""

"""
Standard modules
"""
import os
import sys
import array
from socket import *
from struct import *
import threading
import time
import io
import Queue
import pygtk
pygtk.require('2.0')
import gtk
import gtk.glade
import serial

"""
Initialize Gtk threads
"""
gtk.gdk.threads_init()

"""
Class for all interface processing
"""
class sensor_ui():
   chk_lid = None
   chk_temperature = None
   chk_pressure = None
   chk_airinline = None
   chk_battery = None
   lbl_info = None
   spin_battery = None
   conn_str = None
   buff = [0x10, 0x01, 0x20, 0x0F, 0xFF, 0x30, 0x0F, 0xFF, 0x40, 0x0F, 0xFF, 0x00, 0x50, 0x11]
   
   # UI initialization
   def __init__(self, itemq, conn_str):
      glade_file = ("d:\sandbox\eclipse-workspace\sensor_simulator\src\ui.glade")
      self.wTree = gtk.glade.XML(glade_file)
      
      sig_dict = {"on_btn_set_clicked": self.on_btn_set_clicked, "on_window_destroy": self.destroy}
      
      self.wTree.signal_autoconnect(sig_dict)
      
      self.chk_lid = self.wTree.get_widget("chk_lid")
      self.chk_temperature = self.wTree.get_widget("chk_temperature")
      self.chk_pressure = self.wTree.get_widget("chk_pressure")
      self.chk_airinline = self.wTree.get_widget("chk_airinline")
      self.chk_battery = self.wTree.get_widget("chk_battery")
      self.lbl_info =  self.wTree.get_widget("lbl_info")
      self.spin_battery =  self.wTree.get_widget("spin_battery")

      self.lbl_info.set_text(conn_str)
      
      self.itemq = itemq
      self.itemq.put(self.buff)

   # Gtk event processing loop
   def main(self):
      gtk.gdk.threads_enter()
      gtk.main()
      gtk.gdk.threads_leave()

   # Callback for the "Set" button
   def on_btn_set_clicked(self, widget, data = None):
      # Process all checkboxes
      if self.chk_lid.get_active():
         self.buff[1] = 0x00
      else:
         self.buff[1] = 0x01
            
      if self.chk_pressure.get_active():
         self.buff[3] = 0xFF
         self.buff[4] = 0xFF
      else:
         self.buff[3] = 0x0F
         self.buff[4] = 0xFF
         
      if self.chk_temperature.get_active():
         self.buff[6] = 0xFF
         self.buff[7] = 0xFF
      else:
         self.buff[6] = 0x0F
         self.buff[7] = 0xFF

      if self.chk_battery.get_active():
         self.buff[11] = 0x01
      else:
         self.buff[11] = 0x00
         
      if self.chk_airinline.get_active():
         self.buff[13] = 0x22
      else:
         self.buff[13] = 0x11

      # Read battery percentage, convert to value normalized over 60000
      battery = int(self.spin_battery.get_value()) * (60000 / 100)
      self.buff[9] = battery >> 8
      self.buff[10] = battery & 0xFF      
      
      self.itemq.put(self.buff)
   
   def destroy(self, widget, data = None):
      gtk.main_quit()
      sys.exit(0)
      
"""
The main processing thread for transmitting sensor data
"""
class main_thread(threading.Thread):
   client_socket = None
   host_address = None
   ser_handle = None
   mode = None
   
   def __init__(self, itemq, arg0, arg1, arg2):
      self.itemq = itemq
      
      self.mode = arg0
      
      if self.mode == "udp":
         self.host_address = (arg1, int(arg2))

      if self.mode == "com":
         self.ser_handle = serial.Serial(int(arg1) - 1, int(arg2))
         
      threading.Thread.__init__(self)   

   def run(self):
      q_opt = None
      temp = None

      while 1:
         try:
            q_opt = self.itemq.get(False)
            temp = q_opt

         except Queue.Empty:
            pass

         data = pack("BBBBBBBBBBBBBB",
                     temp[0], temp[1], temp[2],
                     temp[3], temp[4], temp[5],
                     temp[6], temp[7], temp[8],
                     temp[9], temp[10], temp[11],
                     temp[12], temp[13])
      
         if self.mode == "udp":
            self.client_socket = socket(AF_INET, SOCK_DGRAM) 
            self.client_socket.sendto(data, self.host_address)
            self.client_socket.close()
         
         if self.mode == "com":
            self.ser_handle.write(data)
            
         time.sleep(1)

"""
Program entry point
"""
def main():
   if len(sys.argv) != 4:
      sys.stderr.write("Incorrect number of arguments...")
      sys.exit(1)
   
   if (sys.argv[1].find("com") < 0) and (sys.argv[1] != "udp"):
      sys.stderr.write("Incorrect connection type...")
      sys.exit(1)
   
   q = Queue.Queue(3)
   
   conn_str = "Connection:  " + sys.argv[1].upper() + "  [" + sys.argv[2] + " - " + sys.argv[3] + "]"

   ui = sensor_ui(q, conn_str)
   main_thread(q, sys.argv[1], sys.argv[2], sys.argv[3]).start()

   ui.main()

if __name__ == "__main__":
   main()
