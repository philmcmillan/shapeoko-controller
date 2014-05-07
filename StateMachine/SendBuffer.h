// Renamed from Steven Buxton's SendBuffer
// header defining the interface of the source.
#ifndef _SENDBUFFER_H
#define _SENDBUFFER_H

const int BUFFER_SIZE = 20;

// the definition of the queue class.
class SendBuffer {
  public:
    SendBuffer ();
    ~SendBuffer ();

    void push (const unsigned char i);
    unsigned char pop ();
    unsigned char peek () const;
    bool isEmpty () const;
    int count () const;
    bool isFull () const;
    int sum () const;
    void clear ();

  private:
    void blowup (const char * m) const;
    unsigned char contents[BUFFER_SIZE];

    int items;
    int head;
    int tail;
    int total;
};

#endif // _SENDBUFFER_H
