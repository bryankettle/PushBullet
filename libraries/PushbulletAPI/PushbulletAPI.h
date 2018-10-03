/*
Created by: 	Alexander Bryan Sy
Last updated:	23/09/2018
Used to send push notifications to PushBullet
Feel free to edit
*/


#ifndef PushbulletAPI_h
#define PushbulletAPI_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <FS.h> //change this to SD.h for SD card.
using namespace std;

class PushbulletAPI {
	public:
		PushbulletAPI(String accesst); //initialize Pushbullet. requires an access token
		void notify(String title, String body); // sends a push notification
		void notify(String title, String body, String fileurl); //sends a push notification with an image, currently only jpeg is supported. the fileurl is the http address of the image to be sent  
		void PBconnect(); //connects to Pushbullet API
		String upload(String filename); // uploads a file to PushBullet. the output will be the file url
		void sms(String message, String phoneNumber); // sends a SMS. only available in the US
	private:
		String accesstoken; //access token is stored here
		WiFiClientSecure client; //PB client
};


#endif
