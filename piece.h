#ifndef KL_PIECE_H
#define KL_PIECE_H

#include "base/piece.h"


class KLPieceInfo : public GPieceInfo
{
 public:
    KLPieceInfo() {}

    virtual uint nbBlocks() const             { return 0; }
    virtual uint nbTypes() const              { return NB_BLOCK_TYPES; }
    virtual uint nbForms() const              { return 0; }

    virtual const int *i(uint, uint) const    { return 0; }
    virtual const int *j(uint, uint) const    { return 0; }
    virtual uint value(uint, uint) const      { return 0; }
    virtual uint form(uint) const             { return 0; }
    virtual uint nbConfigurations(uint) const { return 0; }

    virtual uint nbNormalBlockTypes() const   { return NB_BLOCK_TYPES; }
    virtual uint nbGarbageBlockTypes() const  { return 0; }
    virtual uint nbBlockModes() const         { return 1+4+6+4+1; }

    virtual uint nbColors() const             { return NB_BLOCK_TYPES; }
    virtual QString colorLabel(uint i) const;
    virtual QColor defaultColor(uint i) const;

 protected:
    void draw(QPixmap *, uint blockType, uint blockMode,
              bool lighted) const;

 private:
    static const uint NB_BLOCK_TYPES = 5;
    static const char *DEFAULT_COLORS[NB_BLOCK_TYPES];
};

#endif



