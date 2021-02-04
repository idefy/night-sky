// DEBUG
// just uncomment #define DEBUG to show debug trace in monitor
//---------------------------------------------------------------

//#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)    Serial.print(x)
 #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTLN(x)
#endif
