# Pushbullet-for-ESP8266
Created by: Alexander Bryan Sy 23/09/2018
feel free to clone and distribute.
Code tested on ESP8266
# How to use
Copy the libraries folder to your Arduino folder

# COMMANDS CURRENTLY AVAILABLE:
 **initialization:**  
    PushbulletAPI(accesstoken);
        
	example:  
		pb.PushbulletAPI(accesstoken);

 **Notifications:**  
    notify(String title, String body);
    
	example:  
		notify("Write the title here","Write the body here");  

notify(String title, String body, String fileurl);
    
	example:  
		notify("Write the title here","Write the body here","https://notarealwebsite.com/images/image.jpg"); 
		//only .jpg files are currently supported

**Upload a File:**  
    upload(String filename);
        
	example:  
		String fileurl = upload("file.jpg"); //currently only jpeg is supported. The output is a HTTP address. 
		the default file storage is SPIFFS. change this to SD if you are using a SD card

**Connect to pushbullet:**  
	connect();
	
	example:  
		connect();//connects to Pushbullet API. not required.

**Send SMS**  
	only available in the US  
	sms(String message, String phoneNumber);
	
	example:
		sms("SMS test", "0917516516");
