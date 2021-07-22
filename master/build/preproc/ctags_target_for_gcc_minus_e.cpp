# 1 "d:\\git\\bomb-diffusal\\master\\master.ino"
# 2 "d:\\git\\bomb-diffusal\\master\\master.ino" 2

byte moduleAddresses[127];
int nModules = 0;

byte strikes = 0;
byte disarmedModules = 0;
# 19 "d:\\git\\bomb-diffusal\\master\\master.ino"
struct ModuleResults
{
 byte strikes = 0;
 byte disarmedModules = 0;
};

void setup()
{
 Serial.println("Master booting");

 Wire.begin();
 Serial.begin(9600);

 // let modules boot first
 delay(100);

 scanI2C();
 setDifficulty(1);
 arm();
}

void setDifficulty(byte difficulty)
{
 Wire.beginTransmission(0);
 Wire.write(2);
 Wire.write(difficulty);
 int error = Wire.endTransmission();
 if(error != 0)
 {
  Serial.print("Error sending set-difficulty command: ");
  Serial.println(error);
 }
}


void arm()
{
 Wire.beginTransmission(0);
 Wire.write(1);
 int error = Wire.endTransmission();
 if(error != 0)
 {
  Serial.print("Error sending arm command: ");
  Serial.println(error);
 }
}

void loop()
{
 ModuleResults results = readModules();

 if(results.strikes != strikes)
 {
  // new strike
  Serial.println("New strike");
 }

 if(results.disarmedModules != disarmedModules)
 {
  // new module disarmed
  Serial.println("New module disarming");
 }

 strikes = results.strikes;
 disarmedModules = results.disarmedModules;
}



ModuleResults readModules()
{
 ModuleResults results;

 for(byte i = 0; i < nModules; i++)
 {
  Wire.requestFrom((int)moduleAddresses[i], 2);
  byte status = Wire.read();
  byte strikes = Wire.read();

  if((status & 2) == 2)
   results.disarmedModules++;

  results.strikes += strikes;
 }

 return results;
}

void scanI2C() {
 Serial.println("Scanning for modules...");
 byte error;
 for(byte address = 1; address < 127; address++)
 {
  Wire.beginTransmission(address);
  error = Wire.endTransmission();

  if (error == 0)
  {
   Serial.print("	Module found at address 0x");
   if (address<16)
    Serial.print("0");
   Serial.println(address, 16);

   moduleAddresses[nModules] = address;
   nModules++;
  }
  else if (error == 4)
  {
   Serial.print("	Unknown error at address 0x");
   if (address<16)
    Serial.print("0");
   Serial.println(address, 16);
  }
 }

 Serial.print("	Found ");
 Serial.print(nModules);
 Serial.println(" module(s)");
 Serial.println("	Finished");
}
