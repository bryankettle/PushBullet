/*
Created by: 	Alexander Bryan Sy
Last updated:	23/09/2018
Used to send push notifications to PushBullet
Feel free to edit
*/
//#define DEBUG

#include "Arduino.h"
#include "PushbulletAPI.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FS.h> ////change this to SD.h for SD card.
using namespace std;
#define PBFINGERPRINT "bbfc9f1bc13cd996f268a2e34129d1478fb933be"
#define HTTPPORT      443
#define PBHOST  "api.pushbullet.com"
String filetype (String filename);


PushbulletAPI::PushbulletAPI(String accesst){//sets up the access token
	accesstoken = accesst;
}

void PushbulletAPI::PBconnect(){
	Serial.print("connecting to ");
  	Serial.println(PBHOST);
  //code for connecting to pushbullet
	if (!client.connect(PBHOST, HTTPPORT)) {
	    Serial.println("connection failed");
	    return;
  	}
  	//checks if the certification matches
  	if (client.verify(PBFINGERPRINT, PBHOST)) {
    	//Serial.println("certificate matches");
    	return;
  	} else {
    	Serial.println("certificate doesn't match");
  	}
}

void PushbulletAPI::notify(String title, String body){// sends a push notification
	PBconnect();
	String url = "/v2/pushes";
	String data = {"{\"body\":\"" + body + "\",\"title\":\"" + title + "\",\"type\":\"note\"}"};
	Serial.print(filetype(body));
	client.print(String("POST ") + url + " HTTP/1.1\r\n" + 
				 "Host: " + PBHOST + "\r\n" + 
				 "User-Agent: ESP8266\r\n" +
				 "Access-Token: " + accesstoken + "\r\n" +
				 "Content-length: " + data.length() + "\r\n" +
				 "Content-Type: application/json\r\n" +
                 "Connection: close\r\n\r\n" + 
                 data
                 );
        #ifdef DEBUG         
	Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
	}
	#endif
} 
void PushbulletAPI::notify(String title, String body,String fileurl){// Sends a notification with an image
	PBconnect();
	String url = "/v2/pushes";
  	String data = {"{\"body\":\""+ body +"\",\"title\":\""+ title +"\",\"type\":\"file\",\"file_type\":\""+ filetype(fileurl) +"\",\"file_url\":\""+fileurl+"\",\"image_width\":322,\"image_height\":600}"};
  	client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + PBHOST + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Access-Token: " + accesstoken + "\r\n" +
               "Content-length:" + data.length() +"\r\n"
               "Content-Type: application/json\r\n" +
               "Connection: close\r\n\r\n" + data
              );


	#ifdef DEBUG
  	Serial.println("request sent");
  	while (client.connected()) {
  	  String line = client.readStringUntil('\n');
  	  if (line == "\r") {
  	    Serial.println("headers received");
  	    break;
   	 	}
	}
	String line = client.readString();
    	Serial.println(line);
	#endif
}
String PushbulletAPI::upload(String filename){// uploads an image. the output will be the upload address
	PBconnect();
	String fileurl,uploadurl,uploadhost;
	String url = "/v2/upload-request"; 
	String data = "{\"file_name\":\"" +filename  + "\",\"file_type\":\"" + filetype(filename) + "\"}";
	client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                    "Host: " + PBHOST + "\r\n" +
                    "User-Agent: ESP8266\r\n" +
                    "Access-Token: " + accesstoken + "\r\n" +
                    "Content-length: " + data.length() + "\r\n" //36 base number
                    "Content-Type: application/json\r\n" +
                    "Connection: close\r\n\r\n" + data
                    
                   );

    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    String line = client.readString();
    int pointer1 = line.indexOf("file_url")+11;
    int pointer2 = line.indexOf("\",",pointer1);
    fileurl = line.substring(pointer1,pointer2);

    pointer1 = line.indexOf("upload_url")+13;
    pointer2 = line.indexOf("\"}",pointer1);
    String upload = line.substring(pointer1,pointer2);

    pointer1 = upload.indexOf("://")+3 ;
    pointer2 = upload.indexOf(".com/")+4;
    uploadhost = upload.substring(pointer1,pointer2);

    pointer1 = upload.indexOf(".com/")+4;
    uploadurl = upload.substring(pointer1);    
    #ifdef DEBUG
    Serial.println("output: "+ line);
    Serial.println(fileurl);
    Serial.println(uploadhost);
    Serial.println(uploadurl);
    #endif
    PBconnect();
    SPIFFS.begin();//change this if your not using SPIFFS
    File imagefile = SPIFFS.open(("/" + filename), "r"); //change this to File imagefile = SD.open((filename), FILE_READ); if your not using SPIFFS
  	data = ("--710ff0c6cf2d4c73b12db64cab12e58c\r\nContent-Disposition:  form-data; name=\"file\"; filename=\""+filename+"\"\r\nContent-Type: "+filetype(filename)+"\r\n\r\n");
	  client.print("POST " +uploadurl + " HTTP/1.1\r\n"+
  				"Host: "+uploadhost+"\r\n"+
			  	"User-Agent: esp8266\r\n"+
				"Access-Token: " + accesstoken + "\r\n" +
  				"Accept-Encoding: gzip, deflate\r\n"+
				"Accept: */*\r\n"+
				"Connection: keep-alive\r\n"+
				"Content-Length: " + (imagefile.size()+40+21+data.length()) + "\r\n" + //40 = "\r\n--710ff0c6cf2d4c73b12db64cab12e58c--\r\n" ; 21 = "Connection: close\r\n\r\n"
				"Content-Type: multipart/form-data;     boundary=710ff0c6cf2d4c73b12db64cab12e58c\r\n" +
				"\r\n" + data);

  	while(imagefile.available()){
    	while(imagefile.position() < imagefile.size()){
      	client.print(dataBuff);
      	client.print("\n");
    	}
  		}
  		client.print(F("\r\n--710ff0c6cf2d4c73b12db64cab12e58c--\r\n"));
  		client.print("Connection: close\r\n\r\n");
	#ifdef DEBUG
    	Serial.println("request sent");
	while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
	#endif
	return (fileurl);    
	imagefile.close();
}

void PushbulletAPI::sms(String message, String phoneNumber){
	PBconnect();
	String url = "/v2/ephemerals";
  	String data = {"{ \"push\": { \"conversation_iden\": \""+phoneNumber+"\",\"message\": \""+message+"\",\"package_name\": \"com.pushbullet.android\",\"source_user_iden\": \""+source_user+"\",\"target_device_iden\": \"phone\",\"type\": \"messaging_extension_reply\"},\"type\": \"push\"}"};
  	client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + PBHOST + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Access-Token: " + accesstoken + "\r\n" +
               "Content-length:" + data.length() +"\r\n"
               "Content-Type: application/json\r\n" +
               "Connection: close\r\n\r\n" + data
              );
	#ifdef DEBUG
  	Serial.println("request sent");
  	while (client.connected()) {
  	  String line = client.readStringUntil('\n');
  	  if (line == "\r") {
  	    Serial.println("headers received");
  	    break;
   	 	}
	}
	#endif
}

String filetype (String filename){ //put the filetypes here
	if(filename.endsWith(".jpg")){
		return ("image/jpeg");
	}
}
