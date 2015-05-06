#ifndef SERVER_SINGLETON_H
#define SERVER_SINGLETON_H

// Should be placed in the appropriate .cpp file somewhere
#define initialiseSingleton( type ) \
  template <> type * Singleton < type > :: mSingleton = 0


template < class type > class  Singleton {
public:
  /// Constructor
  Singleton( ) {
    // If you hit this assert, this singleton already exists -- you can't create another one!
    //CnAssert( mSingleton == 0 );
	mSingleton = static_cast<type *>(this);
  }
  virtual ~Singleton( ) {
	mSingleton = 0;
  }

    inline static type & getSingleton( ) {
      //CnAssert( mSingleton );
      return *mSingleton; }
    inline static type * getSingletonPtr( ) { return mSingleton; }

protected:

  // Singleton pointer, must be set to 0 prior to creating the object
  static type * mSingleton;
};

#endif

