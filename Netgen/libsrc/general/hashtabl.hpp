#ifndef FILE_HASHTABL
#define FILE_HASHTABL

/**************************************************************************/
/* File:   hashtabl.hh                                                    */
/* Author: Joachim Schoeberl                                              */
/* Date:   01. Jun. 95                                                    */
/**************************************************************************/

/**
   Abstract data type HASHTABLE.
   Hash is done by one INDEX
*/
class BASE_INDEX_HASHTABLE
{
protected:
  /// keys are stored in this table
  TABLE<INDEX> hash;
  
public:
  ///
  BASE_INDEX_HASHTABLE (int size)
    : hash (size) { };
  
protected:
  ///
  int HashValue (const INDEX & ind) const
    {
      return ind % hash.Size() + 1;
    }

  ///
  int Position (int bnr, const INDEX & ind) const;
};

///
template <class T>
class INDEX_HASHTABLE : private BASE_INDEX_HASHTABLE
{
  ///
  TABLE<T> cont;
  
public:
  ///
  inline INDEX_HASHTABLE (int size);
  ///
  inline void Set (const INDEX & hash, const T & acont);
  ///
  inline const T & Get (const INDEX & ahash) const;
  ///
  inline int Used (const INDEX & ahash) const;
  ///
  inline int GetNBags () const;
  ///
  inline int GetBagSize (int bnr) const;
  ///
  inline void GetData (int bnr, int colnr, INDEX & ahash, T & acont) const;

  ///
  inline void PrintMemInfo (ostream & ost) const;
};











///
class BASE_INDEX_2_HASHTABLE
{
protected:
  ///
  TABLE<INDEX_2> hash;
  
public:
  ///
  BASE_INDEX_2_HASHTABLE (int size)
    : hash (size) { };

  ///
  void PrintStat (ostream & ost) const;
  void BaseSetSize(int s) {hash.SetSize(s);}
protected:
  ///
  int HashValue (const INDEX_2 & ind) const
    {
      return (ind.I1() + ind.I2()) % hash.Size() + 1;
    }
  ///
  int Position (int bnr, const INDEX_2 & ind) const
  {
    int i;
    for (i = 1; i <= hash.EntrySize (bnr); i++)
      if (hash.Get(bnr, i) == ind)
	return i;
    return 0;
  }
};


///
template <class T>
class INDEX_2_HASHTABLE : public BASE_INDEX_2_HASHTABLE
{
  ///
  TABLE<T> cont;
  
public:
  ///
  inline INDEX_2_HASHTABLE (int size);
  ///
  void SetSize(int s) {cont.SetSize(s); BaseSetSize(s);}
  ///
  inline void Set (const INDEX_2 & ahash, const T & acont)
    {
      int bnr = HashValue (ahash);
      int pos = Position (bnr, ahash);
      if (pos)
	cont.Set (bnr, pos, acont);
      else
	{
	  hash.Add1 (bnr, ahash);
	  cont.Add1 (bnr, acont);
	}    
    }
  ///
  inline const T & Get (const INDEX_2 & ahash) const;
  ///
  inline int Used (const INDEX_2 & ahash) const;
  ///
  inline int GetNBags () const;
  ///
  inline int GetBagSize (int bnr) const;
  ///
  inline void GetData (int bnr, int colnr, 
		       INDEX_2 & ahash, T & acont) const;
  ///
  inline void PrintMemInfo (ostream & ost) const;
};



///
class BASE_INDEX_3_HASHTABLE
{
protected:
  ///
  TABLE<INDEX_3> hash;

public:
  ///
  BASE_INDEX_3_HASHTABLE (int size)
    : hash (size) { };

protected:
  ///
  int HashValue (const INDEX_3 & ind) const
    {
      return (ind.I1() + ind.I2() + ind.I3()) % hash.Size() + 1;
    }

  ///
  int Position (int bnr, const INDEX_3 & ind) const
  {
    const INDEX_3 * pi = &hash.Get(bnr, 1);
    int n = hash.EntrySize(bnr);
    for (int i = 1; i <= n; ++i, ++pi)
      {
	if (*pi == ind)
	return i;
      }
    
    return 0;
  }


};


///
template <class T>
class INDEX_3_HASHTABLE : private BASE_INDEX_3_HASHTABLE
{
  ///
  TABLE<T> cont;

public:
  ///
  inline INDEX_3_HASHTABLE (int size);
  ///
  inline void Set (const INDEX_3 & ahash, const T & acont);
  ///
  inline const T & Get (const INDEX_3 & ahash) const;
  ///
  inline int Used (const INDEX_3 & ahash) const;
  ///
  inline int GetNBags () const;
  ///
  inline int GetBagSize (int bnr) const;
  ///
  inline void SetData (int bnr, int colnr, const INDEX_3 & ahash, const T & acont);
  ///
  inline void GetData (int bnr, int colnr, INDEX_3 & ahash, T & acont) const;
  /// returns position, if not existing, will create (create == return 1)
  inline int PositionCreate (const INDEX_3 & ahash, int & bnr, int & colnr);
  ///
  inline void SetSize (int size);

  ///
  inline void PrepareSet (const INDEX_3 & ahash);
  ///
  inline void AllocateElements ();

  ///
  inline void PrintMemInfo (ostream & ost) const;
  ///
  inline void DeleteData ();
};








/// Closed Hashing HT

class BASE_INDEX_2_CLOSED_HASHTABLE
{
protected:
  ///
  MoveableArray<INDEX_2> hash;
  ///
  int invalid;
public:
  ///
  BASE_INDEX_2_CLOSED_HASHTABLE (int size);

  int Size() const { return hash.Size(); }
  int UsedPos (int pos) const { return ! (hash.Get(pos).I1() == invalid); }
  int UsedElements () const;

  ///
  int HashValue (const INDEX_2 & ind) const
    {
      return (ind.I1() + 71 * ind.I2()) % hash.Size() + 1;
    }


  int Position (const INDEX_2 & ind) const
  {
    int i = HashValue(ind);
    while (1)
      {
	if (hash.Get(i) == ind) return i;
	if (hash.Get(i).I1() == invalid) return 0;
	i++;
	if (i > hash.Size()) i = 1;
      }
    /*
    int pos = HashValue (ind);
    if (hash.Get(pos) == ind) return pos;
    return Position2 (ind);
    */
  }

  // returns 1, if new postion is created
  int PositionCreate (const INDEX_2 & ind, int & apos)
  {
    int i = HashValue (ind);
    if (hash.Get(i) == ind) 
      {
	apos = i;
	return 0;
      }
    if (hash.Get(i).I1() == invalid)
      {
	hash.Elem(i) = ind; 
	apos = i;
	return 1;
      }
    return PositionCreate2 (ind, apos);    
  }

protected:
  ///

  int Position2 (const INDEX_2 & ind) const;
  int PositionCreate2 (const INDEX_2 & ind, int & apos);
  void BaseSetSize (int asize);
};


template <class T>
class INDEX_2_CLOSED_HASHTABLE : public BASE_INDEX_2_CLOSED_HASHTABLE
{
  ///
  MoveableArray<T> cont;

public:
  ///
  inline INDEX_2_CLOSED_HASHTABLE (int size);
  ///
  inline void Set (const INDEX_2 & ahash, const T & acont);
  ///
  inline const T & Get (const INDEX_2 & ahash) const;
  ///
  inline int Used (const INDEX_2 & ahash) const;
  ///
  inline void SetData (int pos, const INDEX_2 & ahash, const T & acont);
  ///
  inline void GetData (int pos, INDEX_2 & ahash, T & acont) const;
  ///
  inline void SetData (int pos, const T & acont);
  ///
  inline void GetData (int pos, T & acont) const;
  ///
  inline void SetSize (int size);
  ///
  inline void PrintMemInfo (ostream & ost) const;
  ///
  inline void DeleteData ()
  { SetSize (cont.Size()); }

  void SetName (const char * aname)
  {
    cont.SetName(aname);
    hash.SetName(aname);
  }
};













class BASE_INDEX_3_CLOSED_HASHTABLE
{
protected:
  ///
  MoveableArray<INDEX_3> hash;
  ///
  int invalid;
public:
  ///
  BASE_INDEX_3_CLOSED_HASHTABLE (int size);

  int Size() const { return hash.Size(); }
  int UsedPos (int pos) const { return ! (hash.Get(pos).I1() == invalid); }
  int UsedElements () const;

  ///
  int HashValue (const INDEX_3 & ind) const
    {
      return (ind.I1() + 15 * ind.I2() + 41 * ind.I3()) % hash.Size() + 1;
    }

  int Position (const INDEX_3 & ind) const
  {
    int pos = HashValue (ind);
    if (hash.Get(pos) == ind) return pos;
    return Position2 (ind);
  }

  // returns 1, if new postion is created
  int PositionCreate (const INDEX_3 & ind, int & apos)
  {
    int i = HashValue (ind);
    if (hash.Get(i) == ind) 
      {
	apos = i;
	return 0;
      }
    if (hash.Get(i).I1() == invalid)
      {
	hash.Elem(i) = ind; 
	apos = i;
	return 1;
      }
    return PositionCreate2 (ind, apos);    
  }


protected:
  ///

  int Position2 (const INDEX_3 & ind) const;
  int PositionCreate2 (const INDEX_3 & ind, int & apos);
  void BaseSetSize (int asize);
};


template <class T>
class INDEX_3_CLOSED_HASHTABLE : public BASE_INDEX_3_CLOSED_HASHTABLE
{
  ///
  MoveableArray<T> cont;

public:
  ///
  inline INDEX_3_CLOSED_HASHTABLE (int size);
  ///
  inline void Set (const INDEX_3 & ahash, const T & acont);
  ///
  inline const T & Get (const INDEX_3 & ahash) const;
  ///
  inline int Used (const INDEX_3 & ahash) const;
  ///
  inline void SetData (int pos, const INDEX_3 & ahash, const T & acont);
  ///
  inline void GetData (int pos, INDEX_3 & ahash, T & acont) const;
  ///
  inline void SetData (int pos, const T & acont);
  ///
  inline void GetData (int pos, T & acont) const;
  ///
  inline void SetSize (int size);
  ///
  inline void PrintMemInfo (ostream & ost) const;
  ///
  inline void DeleteData ()
  { SetSize (cont.Size()); }

  void SetName (const char * aname)
  {
    cont.SetName(aname);
    hash.SetName(aname);
  }
};
















template<class T>
inline INDEX_3_HASHTABLE<T> :: INDEX_3_HASHTABLE (int size)
  : BASE_INDEX_3_HASHTABLE (size), cont(size)
{
  ;
}

template<class T>	
inline int INDEX_3_HASHTABLE<T> :: PositionCreate (const INDEX_3 & ahash, int & bnr, int & colnr)
{
  bnr = HashValue (ahash);
  colnr = Position (bnr, ahash);
  if (!colnr)
    {
      hash.Add (bnr, ahash);
      cont.AddEmpty (bnr);
      colnr = cont.EntrySize (bnr);
      return 1;
    }
  return 0;
}


template<class T>
inline void INDEX_3_HASHTABLE<T> :: Set (const INDEX_3 & ahash, const T & acont)
{
  int bnr = HashValue (ahash);
  int pos = Position (bnr, ahash);
  if (pos)
    cont.Set (bnr, pos, acont);
  else
    {
      hash.Add1 (bnr, ahash);
      cont.Add1 (bnr, acont);
    }
}

template<class T>
inline const T & INDEX_3_HASHTABLE<T> :: Get (const INDEX_3 & ahash) const
{
  int bnr = HashValue (ahash);
  int pos = Position (bnr, ahash);
  return cont.Get (bnr, pos);
}

template<class T>
inline int INDEX_3_HASHTABLE<T> :: Used (const INDEX_3 & ahash) const
{
  return (Position (HashValue (ahash), ahash)) ? 1 : 0;
}

template<class T>
inline int INDEX_3_HASHTABLE<T> :: GetNBags () const
{
  return cont.Size();
}

template<class T>
inline int INDEX_3_HASHTABLE<T> :: GetBagSize (int bnr) const
{
  return cont.EntrySize (bnr);
}

template<class T>
inline void INDEX_3_HASHTABLE<T> :: GetData (int bnr, int colnr, INDEX_3 & ahash, T & acont) const
{
  ahash = hash.Get(bnr, colnr);
  acont = cont.Get(bnr, colnr);
}    

template<class T>
inline void INDEX_3_HASHTABLE<T> :: SetData (int bnr, int colnr, const INDEX_3 & ahash, const T & acont)
{
  hash.Set(bnr, colnr, ahash);
  cont.Set(bnr, colnr, acont);
}    

template<class T>
inline void INDEX_3_HASHTABLE<T> :: SetSize (int size)
{
  hash.SetSize (size);
  cont.SetSize (size);
}

template<class T>
inline void INDEX_3_HASHTABLE<T> :: DeleteData ()
{
  int n = hash.Size();
  hash.SetSize (n);
  cont.SetSize (n);
}

template<class T>
inline void INDEX_3_HASHTABLE<T> :: PrepareSet (const INDEX_3 & ahash)
{
  int bnr = HashValue (ahash);
  hash.IncSizePrepare (bnr-1);
  cont.IncSizePrepare (bnr-1);
}


template<class T>
inline void INDEX_3_HASHTABLE<T> :: AllocateElements ()
{
  hash.AllocateElementsOneBlock();
  cont.AllocateElementsOneBlock();
}



template<class T>
inline void INDEX_3_HASHTABLE<T> :: PrintMemInfo (ostream & ost) const
  {
    ost << "Hash: " << endl;
    hash.PrintMemInfo (ost);
    ost << "Cont: " << endl;
    cont.PrintMemInfo (ost);
  }





template<class T>
inline INDEX_HASHTABLE<T> :: INDEX_HASHTABLE (int size)
  : BASE_INDEX_HASHTABLE (size), cont(size)
  {
    ;
  }
	
template<class T>
inline void INDEX_HASHTABLE<T> :: Set (const INDEX & ahash, const T & acont)
    {
    int bnr = HashValue (ahash);
    int pos = Position (bnr, ahash);
    if (pos)
      cont.Set (bnr, pos, acont);
    else
      {
      hash.Add (bnr, ahash);
      cont.Add (bnr, acont);
      }
    }

template<class T>
inline const T & INDEX_HASHTABLE<T> :: Get (const INDEX & ahash) const
    {
    int bnr = HashValue (ahash);
    int pos = Position (bnr, ahash);
    return cont.Get (bnr, pos);
    }

template<class T>
inline int INDEX_HASHTABLE<T> :: Used (const INDEX & ahash) const
    {
    return (Position (HashValue (ahash), ahash)) ? 1 : 0;
    }

template<class T>
inline int INDEX_HASHTABLE<T> :: GetNBags () const
    {
    return hash.Size();
    }

template<class T>
inline int INDEX_HASHTABLE<T> :: GetBagSize (int bnr) const
    {
    return hash.EntrySize(bnr);
    }

template<class T>
inline void INDEX_HASHTABLE<T> :: GetData (int bnr, int colnr, INDEX & ahash, T & acont) const
    {
    ahash = hash.Get(bnr, colnr);   
    acont = cont.Get(bnr, colnr);
    }
    
template<class T>
inline void INDEX_HASHTABLE<T> :: PrintMemInfo (ostream & ost) const
  {
    ost << "Hash: " << endl;
    hash.PrintMemInfo (ost);
    ost << "Cont: " << endl;
    cont.PrintMemInfo (ost);
  }


    
    
    
    
    
    
    
/****************** INDEX_2_HASHTABLE ****************************/    
    
    
template<class T>
inline INDEX_2_HASHTABLE<T> :: INDEX_2_HASHTABLE (int size)
    : BASE_INDEX_2_HASHTABLE (size), cont(size)
    {
      ;
    }

/*
template<class T>
inline void INDEX_2_HASHTABLE<T> :: Set (const INDEX_2 & ahash, const T & acont)
    {
    int bnr = HashValue (ahash);
    int pos = Position (bnr, ahash);
    if (pos)
      cont.Set (bnr, pos, acont);
    else
      {
      hash.Add (bnr, ahash);
      cont.Add (bnr, acont);
      }
    }
*/

template<class T>
inline const T & INDEX_2_HASHTABLE<T> :: Get (const INDEX_2 & ahash) const
    {
    int bnr = HashValue (ahash);
    int pos = Position (bnr, ahash);
    return cont.Get (bnr, pos);
    }

template<class T>
inline int INDEX_2_HASHTABLE<T> :: Used (const INDEX_2 & ahash) const
    {
    return (Position (HashValue (ahash), ahash)) ? 1 : 0;
    }

template<class T>
inline int INDEX_2_HASHTABLE<T> :: GetNBags () const
    {
    return cont.Size();
    }

template<class T>
inline int INDEX_2_HASHTABLE<T> :: GetBagSize (int bnr) const
    {
    return cont.EntrySize (bnr);
    }

template<class T>
inline void INDEX_2_HASHTABLE<T> :: GetData (int bnr, int colnr, INDEX_2 & ahash, T & acont) const
    {
    ahash = hash.Get(bnr, colnr);
    acont = cont.Get(bnr, colnr);
    }



template<class T>
inline void INDEX_2_HASHTABLE<T> :: PrintMemInfo (ostream & ost) const
  {
    ost << "Hash: " << endl;
    hash.PrintMemInfo (ost);
    ost << "Cont: " << endl;
    cont.PrintMemInfo (ost);
  }













/* *********** Closed Hashing ************************* */




template<class T>
inline INDEX_2_CLOSED_HASHTABLE<T> :: 
INDEX_2_CLOSED_HASHTABLE (int size)
  : BASE_INDEX_2_CLOSED_HASHTABLE(size), cont(size)
{
  cont.SetName ("i2-hashtable, contents");
}

template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
Set (const INDEX_2 & ahash, const T & acont)
{
  int pos;
  PositionCreate (ahash, pos);
  hash.Elem(pos) = ahash;
  cont.Elem(pos) = acont;
}

template<class T>
inline const T & INDEX_2_CLOSED_HASHTABLE<T> :: 
Get (const INDEX_2 & ahash) const
{
  int pos = Position (ahash);
  return cont.Get(pos);
}

template<class T>
inline int INDEX_2_CLOSED_HASHTABLE<T> :: 
Used (const INDEX_2 & ahash) const
{
  int pos = Position (ahash);
  return (pos != 0);
}

template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
SetData (int pos, const INDEX_2 & ahash, const T & acont)
{
  hash.Elem(pos) = ahash;
  cont.Elem(pos) = acont;
}
  
template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
GetData (int pos, INDEX_2 & ahash, T & acont) const
{
  ahash = hash.Get(pos);
  acont = cont.Get(pos);
}

template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
SetData (int pos, const T & acont)
{
  cont.Elem(pos) = acont;
}
  
template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
GetData (int pos, T & acont) const
{
  acont = cont.Get(pos);
}


template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
SetSize (int size)
{
  BaseSetSize(size);
  cont.SetSize(size);
}


  
template<class T>
inline void INDEX_2_CLOSED_HASHTABLE<T> :: 
PrintMemInfo (ostream & ost) const
{
  cout << "Hashtable: " << Size() 
       << " entries of size " << sizeof(INDEX_2) << " + " << sizeof(T) 
       << " = " << Size() * (sizeof(INDEX_2) + sizeof(T)) << " bytes." 
       << " Used els: " << UsedElements() 
       << endl;
}

















template<class T>
inline INDEX_3_CLOSED_HASHTABLE<T> :: 
INDEX_3_CLOSED_HASHTABLE (int size)
  : BASE_INDEX_3_CLOSED_HASHTABLE(size), cont(size)
{
  cont.SetName ("i3-hashtable, contents");
}

template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
Set (const INDEX_3 & ahash, const T & acont)
{
  int pos;
  PositionCreate (ahash, pos);
  hash.Elem(pos) = ahash;
  cont.Elem(pos) = acont;
}

template<class T>
inline const T & INDEX_3_CLOSED_HASHTABLE<T> :: 
Get (const INDEX_3 & ahash) const
{
  int pos = Position (ahash);
  return cont.Get(pos);
}

template<class T>
inline int INDEX_3_CLOSED_HASHTABLE<T> :: 
Used (const INDEX_3 & ahash) const
{
  int pos = Position (ahash);
  return (pos != 0);
}

template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
SetData (int pos, const INDEX_3 & ahash, const T & acont)
{
  hash.Elem(pos) = ahash;
  cont.Elem(pos) = acont;
}
  
template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
GetData (int pos, INDEX_3 & ahash, T & acont) const
{
  ahash = hash.Get(pos);
  acont = cont.Get(pos);
}

template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
SetData (int pos, const T & acont)
{
  cont.Elem(pos) = acont;
}
  
template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
GetData (int pos, T & acont) const
{
  acont = cont.Get(pos);
}


template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
SetSize (int size)
{
  BaseSetSize(size);
  cont.SetSize(size);
}


  
template<class T>
inline void INDEX_3_CLOSED_HASHTABLE<T> :: 
PrintMemInfo (ostream & ost) const
{
  cout << "Hashtable: " << Size() 
       << " entries of size " << sizeof(INDEX_3) << " + " << sizeof(T) 
       << " = " << Size() * (sizeof(INDEX_3) + sizeof(T)) << " bytes" << endl;
}






#endif
