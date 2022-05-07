# firearrow

[![Apk file](https://img.shields.io/github/downloads/genuinetools/apk-file/total.svg?style=for-the-badge)](files/app-debug.apk)

Android application for test and measure Java/C++ features.

The purpose of the application is to test, compare and measure time execution</br> 
 Java/C++ simialar functionalities.

![List of features](files/firearrow_1.jpg "A list of features")
![Filesystem feature](files/firearrow_2.jpg "A erase file feature")
![Async feature](files/firearrow_3.jpg "A asynchronously feature")
![Simd feature](files/firearrow_4.jpg "A simd feature")

Features:
---------
  * filesystem - compare Java vs C++ erase file facilities
  * async      - compare Java vs C++ asynchronously facilities
  * simd       - compare Java vs C++ vector instruction facilities
  * network    - compare Java vs C++ network facilities

Architecture:
-------------
MVVM (Model View ViewModel)

Used Android libraries:
-----------------------
  * AppCompat
  * App Startup
  * Lifecycle/LiveData
  * ViewModel
  * Pagging
  * Navigation
  * Room
  * DataStore
  * WorkManager
  
Used third party libraries:
--------------------------
  * Timber
  * Glide
  * Hilt
  * RxJava3

Languages:
----------
  * Java 11
  * C++ 20
  
