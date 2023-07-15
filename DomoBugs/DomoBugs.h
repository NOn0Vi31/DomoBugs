struct sUpdateStateReq {
  String VariableName;
  String RemoteURI;
};
const int MaximumNumberOfRequest = 20;
sUpdateStateReq UpdateStateRequest[MaximumNumberOfRequest];
int NumberOfDeclaredRequest = 0;