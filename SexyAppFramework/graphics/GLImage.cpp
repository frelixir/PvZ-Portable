#include "GLImage.h"
#include "misc/CritSect.h"
#include "misc/Debug.h"
#include "GLInterface.h"
#include "SexyAppBase.h"
#include "Image.h"

using namespace Sexy;

GLImage::GLImage() : MemoryImage(gSexyAppBase)
{
	mGLInterface = gSexyAppBase->mGLInterface;
	mGLInterface->AddGLImage(this);
}

GLImage::GLImage(GLInterface *theGLInterface) : MemoryImage(theGLInterface->mApp)
{
	mGLInterface = theGLInterface;
	mGLInterface->AddGLImage(this);
}

GLImage::~GLImage()
{
	mGLInterface->RemoveGLImage(this);
}

bool GLImage::Check3D(GLImage *theImage)
{
	return true;
}

bool GLImage::Check3D(Image *theImage)
{
	GLImage *anImage = dynamic_cast<GLImage*>(theImage);
	return anImage != 0;
}

bool GLImage::PolyFill3D(const Point theVertices[], int theNumVertices, const Rect *theClipRect, const Color &theColor, int theDrawMode, int tx, int ty)
{
	mGLInterface->FillPoly(theVertices,theNumVertices,theClipRect,theColor,theDrawMode,tx,ty);
	return true;
}

void GLImage::FillRect(const Rect& theRect, const Color& theColor, int theDrawMode)
{
	mGLInterface->FillRect(theRect,theColor,theDrawMode);
}

void GLImage::DrawLine(double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode)
{
	mGLInterface->DrawLine(theStartX,theStartY,theEndX,theEndY,theColor,theDrawMode);
}

void GLImage::DrawLineAA(double theStartX, double theStartY, double theEndX, double theEndY, const Color& theColor, int theDrawMode)
{
	mGLInterface->DrawLine(theStartX,theStartY,theEndX,theEndY,theColor,theDrawMode);
}

void GLImage::Blt(Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode)
{
	theImage->mDrawn = true;

	//if (gDebug)
	//	mApp->CopyToClipboard("+DDImage::Blt");

	DBG_ASSERTE((theColor.mRed >= 0) && (theColor.mRed <= 255));
	DBG_ASSERTE((theColor.mGreen >= 0) && (theColor.mGreen <= 255));
	DBG_ASSERTE((theColor.mBlue >= 0) && (theColor.mBlue <= 255));
	DBG_ASSERTE((theColor.mAlpha >= 0) && (theColor.mAlpha <= 255));

	CommitBits();

	mGLInterface->Blt(theImage,theX,theY,theSrcRect,theColor,theDrawMode);
}

void GLImage::BltF(Image* theImage, float theX, float theY, const Rect& theSrcRect, const Rect &theClipRect, const Color& theColor, int theDrawMode)
{
	theImage->mDrawn = true;

	FRect aClipRect(theClipRect.mX,theClipRect.mY,theClipRect.mWidth,theClipRect.mHeight);
	FRect aDestRect(theX,theY,theSrcRect.mWidth,theSrcRect.mHeight);

	FRect anIntersect = aDestRect.Intersection(aClipRect);
	if (anIntersect.mWidth!=aDestRect.mWidth || anIntersect.mHeight!=aDestRect.mHeight)
	{
		if (anIntersect.mWidth!=0 && anIntersect.mHeight!=0)
			mGLInterface->BltClipF(theImage,theX,theY,theSrcRect,&theClipRect,theColor,theDrawMode);
	}
	else
		mGLInterface->Blt(theImage,theX,theY,theSrcRect,theColor,theDrawMode,true);
}

void GLImage::BltRotated(Image* theImage, float theX, float theY, const Rect &theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, double theRot, float theRotCenterX, float theRotCenterY)
{
	theImage->mDrawn = true;

	//if (mNoLock)
		//return;	

	CommitBits();

	mGLInterface->BltRotated(theImage,theX,theY,&theClipRect,theColor,theDrawMode,theRot,theRotCenterX,theRotCenterY,theSrcRect);
}

void GLImage::StretchBlt(Image* theImage, const Rect& theDestRect, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool fastStretch)
{
	theImage->mDrawn = true;

	CommitBits();

	mGLInterface->StretchBlt(theImage,theDestRect,theSrcRect,&theClipRect,theColor,theDrawMode,fastStretch);
}

void GLImage::BltMatrix(Image* theImage, float x, float y, const SexyMatrix3 &theMatrix, const Rect& theClipRect, const Color& theColor, int theDrawMode, const Rect &theSrcRect, bool blend)
{
	theImage->mDrawn = true;

	mGLInterface->BltTransformed(theImage,&theClipRect,theColor,theDrawMode,theSrcRect,theMatrix,blend,x,y,true);
}

void GLImage::BltTrianglesTex(Image *theTexture, const TriVertex theVertices[][3], int theNumTriangles, const Rect& theClipRect, const Color &theColor, int theDrawMode, float tx, float ty, bool blend)
{
	theTexture->mDrawn = true;

	mGLInterface->DrawTrianglesTex(theVertices,theNumTriangles,theColor,theDrawMode,theTexture,tx,ty,blend);
}

void GLImage::BltMirror(Image* theImage, int theX, int theY, const Rect& theSrcRect, const Color& theColor, int theDrawMode)
{
	DBG_ASSERTE((theColor.mRed >= 0) && (theColor.mRed <= 255));
	DBG_ASSERTE((theColor.mGreen >= 0) && (theColor.mGreen <= 255));
	DBG_ASSERTE((theColor.mBlue >= 0) && (theColor.mBlue <= 255));
	DBG_ASSERTE((theColor.mAlpha >= 0) && (theColor.mAlpha <= 255));

	CommitBits();

	mGLInterface->BltMirror(theImage,theX,theY,theSrcRect,theColor,theDrawMode);
}

void GLImage::StretchBltMirror(Image* theImage, const Rect& theDestRectOrig, const Rect& theSrcRect, const Rect& theClipRect, const Color& theColor, int theDrawMode, bool fastStretch)
{
	theImage->mDrawn = true;

	CommitBits();

	mGLInterface->StretchBlt(theImage,theDestRectOrig,theSrcRect,&theClipRect,theColor,theDrawMode,fastStretch,true);
}
