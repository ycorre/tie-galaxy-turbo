"
I am a static animation always returning the same sprite frame.
"
Class {
	#name : 'MolStaticAnimation',
	#superclass : 'Object',
	#instVars : [
		'currentFrame'
	],
	#category : 'Molecular-model - 2D graphics',
	#package : 'Molecular',
	#tag : 'model - 2D graphics'
}

{ #category : 'accessing' }
MolStaticAnimation class >> defaultName [
	^'static'
]

{ #category : 'building' }
MolStaticAnimation >> buildFramesInBounds: aRectangle [ 
	currentFrame := aRectangle asMolSpriteFrame 
]

{ #category : 'accessing' }
MolStaticAnimation >> currentFrame [

	^ currentFrame
]

{ #category : 'accessing' }
MolStaticAnimation >> currentFrame: anObject [

	^ currentFrame := anObject
]

{ #category : 'accessing' }
MolStaticAnimation >> name [

	^ self class defaultName
]

{ #category : 'animation' }
MolStaticAnimation >> nextFrame [

	^ currentFrame
]

{ #category : 'printing' }
MolStaticAnimation >> printOn: aStream [

	aStream << 'Molecular animation:'.
	aStream space.
	aStream << self name
]
