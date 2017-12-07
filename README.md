# visualPi
Raspberry Pi 3 with a small screen to display IP address and WiFi information 

I like to use Raspberry Pi to run some interesting code. But it is really annoying when I need to get the IP address info for SSH connection( I don't have monitor for my Pi). I am starting to think about show the network info through a small OLED screen it would be easy to work everywhere. Then I have visualPi. 

![Raspberry PI 3 with OLED](https://github.com/haoranstone/visualPi/blob/master/pictures/model.jpg)


#Hardware connection  
  | OLED   -----------    PI 3   |  
  | GND       <---->      GND    |  
  | VCC       <---->      VCC    |  
  | SCL       <---->    SPI0.SCL | (Header 23)  
  | SDA       <---->    SPI0.MOSI| (Header 19)  
  | RST       <---->    GPIO5    |  
  | D/C       <---->    GPIO1    |  
  
  #Compile and configuration
  ```shell
  cd /opt/visualpi
  make 
  echo "/opt/visualpi" >> /etc/rc.local
  ```
  after configuration, OLED will be launched once the system is powered up. IP of eth0 and wlan0 will be freshed every 5 seconds.
