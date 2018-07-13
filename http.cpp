
#include "stdafx.h"
#include <stdio.h> 
#include <stdlib.h>
#include <wchar.h>

#using <System.dll> //using C++/CLR libraries as in C# 

using namespace std;
using namespace System;
using namespace System::Net;
using namespace System::Text;
using namespace System::IO;

#include "http.h"
#include <string.h>

Http::Http() {
	this->reset();
	this->url = "http://shed-pi/cgi-bin/tempjson.cgi";
}

Http::Http( char* alternate_url) {
	this->reset();
	this->url = alternate_url;
}


void Http::reset() {
	this->temperature = 0.0;
	this->humidity = 0.0;
	this->sensor_status = -1;
	this->data_valid = false;
	
}


void Http::validate_sensor_data() {
	this->data_valid = sensor_status == 0;
}

float Http::arr_to_float( array<Char>^ Data) {
	char num[6];
	for (int i = 0; i < 6; i++)
		num[i] = (char)Data[i];

	return atof(num);
}

void Http::read_data() {

	this->reset();

	System::String ^s = gcnew System::String(this->url);

	try {
		HttpWebRequest ^first_request = safe_cast<HttpWebRequest^>(HttpWebRequest::Create(s)); //Create a HttpWebRequest object 
		first_request->Method = "GET";//Set the Method 
		first_request->KeepAlive = true;//Set Keep Alive 
		HttpWebResponse ^response = safe_cast<HttpWebResponse^>(first_request->GetResponse());//Get the Response  
		Stream^ receiveStream = response->GetResponseStream();//Read Response 
		StreamReader^ readStream = gcnew StreamReader(receiveStream, Encoding::UTF8);


		int current_char = 0;

		StringBuilder^ new_data = gcnew StringBuilder();

		array<Char>^H = gcnew array<Char>(6);
		array<Char>^C = gcnew array<Char>(6);


		while (readStream->Peek() >= 0) {
			current_char = readStream->Read();
			if (current_char == 44 || current_char == 46 || (current_char >= 48 && current_char <= 57)) new_data->Append(Char(current_char));
		}

		new_data->CopyTo(0, H, 0, 5);
		new_data->CopyTo(6, C, 0, 5);

		this->humidity = this->arr_to_float(H);
		this->temperature = this->arr_to_float(C);

		response->Close();
		readStream->Close();
		this->sensor_status = 0;
	}
	catch (WebException ^ex)
	{
		this->sensor_status = 10;
	}

	this->validate_sensor_data();
}
