#include "SendBuffer.h"
#include <Arduino.h>

SendBuffer::SendBuffer () {
  items = 0;
  head = 0;
  tail = 0;
  total = 0;
}

SendBuffer::~SendBuffer () {
  items = 0;
  head = 0;
  tail = 0;
  total = 0;
}

void SendBuffer::clear () {
  items = 0;
  head = 0;
  tail = 0;
  total = 0;
}

void SendBuffer::push (const unsigned char i) {
  if (isFull ())
    blowup ((char*)F("Buffer FULL!"));
  contents[tail++] = i;
  if (tail == BUFFER_SIZE) tail = 0;
  items++;
  total = total + i;
}

unsigned char SendBuffer::pop () {
  if (isEmpty ())
    blowup ((char*)F("Nothing to POP!"));

  unsigned char item = contents[head++];
  items--;
  if (head == BUFFER_SIZE) head = 0;
  total = total - item;
  return item;
}

unsigned char SendBuffer::peek () const {
  if (isEmpty ())
    blowup ((char*)F("Nothing to PEEK!"));

  return contents[head];
}

bool SendBuffer::isEmpty () const {
  return items == 0;
}

bool SendBuffer::isFull () const {
  return items == BUFFER_SIZE;
}

int SendBuffer::count () const {
  return items;
}

int SendBuffer::sum () const {
  return total;
}

void SendBuffer::blowup (const char * m) const {
  Serial.print(m);
  while(true)
  {}
}
