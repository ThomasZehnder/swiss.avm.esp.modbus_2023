#include "LinkedList.h"

// get from https://github.com/KeeganMartin/Dictionary

template <class T, class U>
class Dictionary
{
private:
    LinkedList<T> KeyList = LinkedList<T>();
    LinkedList<U> ValList = LinkedList<U>();

public:
    void set(T key, U val)
    {
        for (int i = 0; i < KeyList.size(); i++)
        {
            if (KeyList.get(i) == key)
            {
                ValList.set(i, val);
                return;
            }
        }
        KeyList.add(key);
        ValList.add(val);
    }

    U get(T key)
    {
        for (int i = 0; i < KeyList.size(); i++)
        {
            if (KeyList.get(i) == key)
            {
                return ValList.get(i);
            }
        }
        return "ndef";
    }
    U getByIndex(int i)
    {
        return ValList.get(i);
    }
    
    T getKey(U val)
    {
        for (int i = 0; i < ValList.size(); i++)
        {
            if (ValList.get(i) == val)
            {
                return KeyList.get(i);
            }
        }
        return "ndef";
    }

    T getKeyByIndex(int i)
    {
        return KeyList.get(i);
    }

    int length()
    {
        return KeyList.size();
    }

    void clear()
    {
       ValList.clear();
       KeyList.clear(); 
    }
};