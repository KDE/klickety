#ifndef KL_PIECE_H
#define KL_PIECE_H

#include "base/piece.h"


class KLPieceInfo : public GPieceInfo
{
 public:
	KLPieceInfo() {}

	uint nbBlocks() const             { return 0; }
	uint nbTypes() const              { return NB_BLOCK_TYPES; }
	uint nbForms() const              { return 0; }

	const int *i(uint, uint) const    { return 0; }
	const int *j(uint, uint) const    { return 0; }
	uint value(uint, uint) const      { return 0; }
	uint form(uint) const             { return 0; }
	uint nbConfigurations(uint) const { return 0; }

	uint nbNormalBlockTypes() const   { return NB_BLOCK_TYPES; }
    uint nbGarbageBlockTypes() const  { return 0; }
	uint nbBlockModes() const         { return 1+4+6+4+1; }

    uint nbColors() const { return NB_BLOCK_TYPES; }

 protected:
    void draw(QPixmap *, uint blockType, uint blockMode,
              bool lighted) const;

 private:
    static const uint NB_BLOCK_TYPES = 5;
};

#endif



