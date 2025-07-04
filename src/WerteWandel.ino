



bool StringVergleich(String A, String B)
{
  return A.equals(B);
}

String float_to_String(float f)
{
   char buffer[6];
   sprintf(buffer, "%.1f", f);  
  return buffer;
}

float String_to_float(String s)
{
  return s.toFloat();
}

String String_laenge_ergaenzen(String S, byte N) // Überschriften zentrieren und mit Füllzeichen versehen
{
  char Fuellzeichen = '-';
  S = ' ' + S + ' ';
  unsigned int L = S.length();
  byte A = (N - L)/2;
  byte B = N - L - A;
  String tmp = "";
  for (byte i; i<A ; i++) tmp = tmp + Fuellzeichen;
  tmp = tmp + S;
  for (byte i; i<B ; i++) tmp = tmp + Fuellzeichen;
  return tmp;
}

int String_to_int( String S)
{
  return  S.toInt();
}

bool Bool(String s)  // wandelt einen String zu einem Bool 
{
  int i = s.toInt();
  if (i > 0) return true; else return false;
}
