#ifndef ALIALIGNOBJ_H
#define ALIALIGNOBJ_H

/*************************************************************************
 * AliAlignObj: alignment base class for the storage of alignment        *
 *   information for a single volume, that is a translation, a rotation  *
 *   and a the identity of the volume itself in form of a TGeo path and  *
 *   as a unique integer identifier                                      *
 *************************************************************************/
#include "TObject.h"
#include "TString.h"
#include "TGeoMatrix.h"

class AliAlignObj : public TObject {

 public:

  AliAlignObj();
  AliAlignObj(const AliAlignObj& theAlignObj);
  AliAlignObj& operator= (const AliAlignObj& theAlignObj);
  virtual ~AliAlignObj();

  //Setters
  virtual void SetTranslation(Double_t x, Double_t y, Double_t z) = 0;
  virtual void SetTranslation(const TGeoMatrix& m) = 0;
  virtual void SetRotation(Double_t psi, Double_t theta, Double_t phi) = 0;
  virtual Bool_t SetRotation(const TGeoMatrix& m) = 0;
  virtual void SetPars(Double_t x, Double_t y, Double_t z, Double_t psi,
               Double_t theta, Double_t phi) = 0;
  virtual void SetMatrix(const TGeoMatrix& m) = 0;
  void  SetVolPath(const TString& volpath) {fVolPath=volpath;}
  void  SetVolUID(const UShort_t voluid) {fVolUID=voluid;}

  //Getters
  const char  *GetVolPath()    const {return fVolPath.Data();}
  UShort_t     GetVolUID()     const {return fVolUID;}
  virtual void GetTranslation(Double_t* tr)  const=0;
  virtual Bool_t GetAngles(Double_t* angles) const=0;
  virtual void GetPars(Double_t transl[], Double_t rot[]) const=0;
  virtual void GetMatrix(TGeoHMatrix& m) const=0;

  void  Print(Option_t *) const;

 protected:

  void AnglesToMatrix(const Double_t *angles, Double_t *rot) const;
  Bool_t MatrixToAngles(const Double_t *rot, Double_t *angles) const;

  //Volume identifiers
  TString fVolPath; // Volume path inside TGeo geometry
  UShort_t fVolUID; // Unique volume ID
  
  ClassDef(AliAlignObj, 1)
};

/*****************************************************************************
 * AliAlignObjAngles: derived alignment class storing alignment information  *
 *   for a single volume in form of three doubles for the translation        *
 *   and three doubles for the rotation expressed with the euler angles      *
 *   in the xyz-convention (http://mathworld.wolfram.com/EulerAngles.html),  *
 *   also known as roll, pitch, yaw. PLEASE NOTE THE ANGLES SIGNS ARE        *
 *   INVERSE WITH RESPECT TO THIS REFERENCE!!! In this way the representation*
 *   is fully consistent with the TGeo Rotation methods.                     *
 *****************************************************************************/
class AliAlignObjAngles : public AliAlignObj{
 public:
  AliAlignObjAngles();
  AliAlignObjAngles(const AliAlignObjAngles& theAlignObj);
  AliAlignObjAngles& operator= (const AliAlignObjAngles& theAlignObj);
  virtual ~AliAlignObjAngles();
  
  //Setters
  virtual void SetTranslation(Double_t x, Double_t y, Double_t z){
    fTranslation[0]=x; fTranslation[1]=y; fTranslation[2]=z;}
  virtual void SetTranslation(const TGeoMatrix& m);
  virtual void SetRotation(Double_t psi, Double_t theta, Double_t phi){
    fRotation[0]=psi; fRotation[1]=theta; fRotation[2]=phi;}
  virtual Bool_t SetRotation(const TGeoMatrix& m);
  virtual void SetMatrix(const TGeoMatrix& m);
  virtual void SetPars(Double_t x, Double_t y, Double_t z, Double_t psi,
		   Double_t theta, Double_t phi){
    fTranslation[0]=x; fTranslation[1]=y; fTranslation[2]=z;
    fRotation[0]=psi; fRotation[1]=theta; fRotation[2]=phi;}
  
  //Getters
  virtual void GetTranslation(Double_t *tr)  const {
    tr[0] = fTranslation[0]; tr[1] = fTranslation[1]; tr[2] = fTranslation[2];}
  virtual Bool_t GetAngles(Double_t* angles)   const {
    angles[0] = fRotation[0]; angles[1] = fRotation[1];
    angles[2] = fRotation[2]; return kTRUE;}
  virtual void GetPars(Double_t tr[], Double_t angles[]) const;
  virtual void GetMatrix(TGeoHMatrix& m) const;
  
 protected:
  Double_t fTranslation[3]; // Translation vector
  Double_t fRotation[3]; // Roll-pitch-yaw angles
  
  ClassDef(AliAlignObjAngles, 1)
};

/**************************************************************************
 * AliAlignObjMatrix: derived alignment class storing alignment           *
 *   information for a single volume in form of TGeoHMatrix, which        *
 *   includes the information for a translation, a rotation and a scale   *
 *************************************************************************/
class AliAlignObjMatrix : public AliAlignObj {
 public:
  AliAlignObjMatrix();
  AliAlignObjMatrix(const AliAlignObjMatrix& theAlignObj);
  AliAlignObjMatrix& operator= (const AliAlignObjMatrix& theAlignObj);
  virtual ~AliAlignObjMatrix();
  
  //Setters
  virtual void SetTranslation(Double_t x, Double_t y, Double_t z);
  virtual void SetTranslation(const TGeoMatrix& m);
  virtual void SetRotation(Double_t psi, Double_t theta, Double_t phi);
  virtual Bool_t SetRotation(const TGeoMatrix& m);
  virtual void SetMatrix(const TGeoMatrix& m);
  virtual void SetPars(Double_t x, Double_t y, Double_t z, Double_t psi,
		   Double_t theta, Double_t phi);
  //Getters
  virtual void GetTranslation(Double_t* tr)  const;
  virtual Bool_t GetAngles(Double_t* angles)  const;
  virtual void GetPars(Double_t tr[], Double_t rot[]) const;
  virtual void GetMatrix(TGeoHMatrix& m) const;
  
 protected:
  TGeoHMatrix fMatrix; // Transformation matrix
  
  ClassDef(AliAlignObjMatrix, 1)
};

#endif
