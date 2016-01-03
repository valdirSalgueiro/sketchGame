/*
 *  PlaceObject.cpp
 *  MonkSWF
 *
 *  Created by Micah Pearlman on 3/26/09.
 *  Copyright 2009 MP Engineering. All rights reserved.
 *
 */

#include "PlaceObject.h"
#include "mkSWF.h"
#include <iostream>
using namespace std;

namespace MonkSWF {
	
	bool PlaceObject2Tag::read( Reader* reader ) {
	
		if( reader->getBitPos() )
			cout << "UNALIGNED!" << endl;
		uint8_t has_clip_actions = reader->getbits( 1 );
		uint8_t has_clip_depth = reader->getbits( 1 );
		uint8_t has_name = reader->getbits( 1 );
		uint8_t has_ratio = reader->getbits( 1 );
		uint8_t has_color_transform = reader->getbits( 1 );
		_has_matrix = reader->getbits( 1 );
		_has_character = reader->getbits( 1 );
		_do_move = reader->getbits( 1 );
		
		if( has_clip_actions || has_clip_depth )
			assert( 0 );
			
		_depth = reader->get<uint16_t>();
		if( _has_character )
			_character_id = reader->get<uint16_t>();
			
			
		if( _has_matrix ) {
			MATRIX m;
			reader->getMatrix( m );
			
			//	OpenVG:
			//	sx	shx	tx
			//	shy	sy	ty
			//	0	0	1
			
			// also see MATRIX in swf file format spec
			
			// translation
			_transform[0][2] = m.tx/20.0f;
			_transform[1][2] = m.ty/20.0f;
			
			// scale
			_transform[0][0] = m.sx/65536.0f;
			_transform[1][1] = m.sy/65536.0f;
			
			// rotate and skew
			// note the order of r1 and r0!
			_transform[0][1] = m.r1/65536.0f;
			_transform[1][0] = m.r0/65536.0f;
			
			
		}
		
		if ( has_color_transform ) {
		
			const uint8_t has_add_terms = reader->getbits( 1 );
			const uint8_t has_mult_terms = reader->getbits( 1 );
			const uint8_t nbits = reader->getbits( 4 );
			if( has_mult_terms ) {
				coord_t red = reader->getsignedbits( nbits );
				coord_t green = reader->getsignedbits( nbits );
				coord_t blue = reader->getsignedbits( nbits );
				coord_t alpha = reader->getsignedbits( nbits );

			}
			
			if( has_add_terms ) {
				coord_t red = reader->getsignedbits( nbits );
				coord_t green = reader->getsignedbits( nbits );
				coord_t blue = reader->getsignedbits( nbits );
				coord_t alpha = reader->getsignedbits( nbits );
			}
		}
		
		if ( has_ratio ) {
			// dummy read
			// TODO
			reader->get<uint16_t>();
		}
		if( has_name ) {
			char* n = reader->getString();
			_name = std::string( n );
			delete [] n;
		}
		if( has_clip_depth ) {
			reader->get<uint16_t>();
		}
		if( has_clip_actions ) {
			// TOOD: support clip actions 
			assert( 0 );
		}
		
		return true;
	}
	
	void PlaceObject2Tag::copyTransform( IPlaceObjectTag* o ) {
		//copyNoTransform( o );
		PlaceObject2Tag* other = (PlaceObject2Tag*)o;
		for ( int i = 0; i < 3; i++ ) {
			for ( int p = 0; p < 3; p++ ) {
				_transform[i][p] = other->_transform[i][p];
			}
		}
	}
	
	static inline float degrees (float radians) {return radians * (180.0f/3.14159f);}	
	void PlaceObject2Tag::draw( SWF* swf ) {
		VGfloat oldMatrix[9];
		vgGetMatrix( oldMatrix );
		vgMultMatrix( (VGfloat*)&_transform[0][0] );
		vgScale( _offsetScale, _offsetScale );
		vgTranslate( _offsetTranslate[0], _offsetTranslate[1] );
		
		IDefineShapeTag* shape = swf->getShape( _character_id );
		if( shape )
			shape->draw();
		else {
			IDefineSpriteTag* sprite = swf->getSprite( _character_id );
			if ( sprite ) {
				sprite->draw(0);		// TOOD: have swf store the current frame
			}
		}
		// restore old matrix
		vgLoadMatrix( oldMatrix );

	}
	
	void PlaceObject2Tag::print() {
		cout << "PLACEOBJECT2: id = " << _character_id << " depth = " << _depth << " DoMove = " << doMove() << endl;
		_header.print();
	}

}