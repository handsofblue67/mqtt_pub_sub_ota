class Device {
  string type;
  string id;
  unsigned int deviceState;
  unsigned int reportInterval;
  bool reportOnChange;

  Device() {
    this.type = "toggleable";
    this.id = CLIENT_ID;
    this.deviceState = LOW;
    this.reportInterval = 600000; // default to report every hour
    this.reportOnChange = false;
  }
  
  Device(string type, string id, unsigned int deviceState, unsigned int reportInterval, bool reportOnChange) {
    this.type = type;
    this.id = id;
    this.deviceState = deviceState;
    this.reportInterval = reportInterval;
    this.reportOnChange = reportOnChange;
  }
 public:
  string getType() {
    return this.type;
  }

  void setType(string type) {
    this.type = type;
  }

  string getId() {
    return this.id;
  }

  void setId(string id) {
    this.id = id;
  }

  unsigned int getDeviceState() {
    return this.state;
  }
  
  void setDeviceState(unsigned int deviceState) {
    this.deviceState = deviceState;
  }

  unsigned int getReportInterval() {
    return this.reportInterval;
  }

  void setReportInterval(unsigned int reportInterval) {
    this.reportInterval = reportInterval;
  }

  bool getReportOnChange() {
    return this.reportOnChange;
  }

  void setReportOnChange(bool reportOnChange) {
    this.reportOnChange = reportOnChange;
  }
}
