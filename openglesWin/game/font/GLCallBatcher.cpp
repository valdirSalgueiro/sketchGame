#include "PreCompile.h"
#include "GLCallBatcher.h"
#include "ResManager.h"
#include "FTBitmapChar.h"

const int dataPerVextex = 5;
GLCallBatcher::GLCallBatcher()
{
	m_numQuads = 0;
	m_pVertexData = NULL;
	m_pIndices = NULL;
	m_textureID = 0;
	m_sizeCache = 0;
	m_drawCallCount = 0;
	// assume that we will have at least 150 quads strung together
	Reallocate(150);
}

GLCallBatcher::~GLCallBatcher()
{
	Release();
}

// deletes memory for arrays
void GLCallBatcher::Release()
{
	if (m_pVertexData != NULL)
	{
		delete[] m_pVertexData;
		m_pVertexData = NULL;
	}
	if (m_pIndices != NULL)
	{
		delete[] m_pIndices;
		m_pIndices = NULL;
	}
	m_sizeCache = 0;
}

// adds a quad of a give colour, alpha, etc
void GLCallBatcher::AddQuad(GLuint textureID, int color, float alpha, bool transparent, bool smooth,
	const float* pTexCoords, const float* pVertices) 
{
	SetAttribs(textureID, color, alpha, transparent, smooth);
	AddQuad(pTexCoords, pVertices);
}	// Render

// adds a quad and assumes that the colour, alpha is same as previous
void GLCallBatcher::AddQuad(const float* pTexCoords, const float* pVertices) 
{
	if (m_numQuads >= m_sizeCache)
	{
		Reallocate(20);
	}
	
	int currIndex = m_numQuads*verticesPerQuad*dataPerVextex;
	for (int n = 0; n < verticesPerQuad; n++)
	{
		// x,y,z: note: do not need to copy 3rd coordinate, just zero it
		m_pVertexData[currIndex+n*dataPerVextex] = pVertices[n*compVertPos];
		m_pVertexData[currIndex+n*dataPerVextex+1] = pVertices[n*compVertPos+1];
		// u,v
		m_pVertexData[currIndex+n*dataPerVextex+3] = pTexCoords[n*compVertTex];
		m_pVertexData[currIndex+n*dataPerVextex+4] = pTexCoords[n*compVertTex+1];
	}
	m_numQuads++;
}	// Render

// sets the current colour, alpha, etc,
// if this is different than previous then the current quads have to be rendered
// before adding any new ones
void GLCallBatcher::SetAttribs(GLuint textureID, int color, float alpha, bool transparent, bool smooth) 
{
	if (alpha > 1.0f) alpha = 1.0f;
	if (m_textureID != textureID || m_color != color || m_alpha != alpha ||
		m_transparent != transparent || m_smooth != smooth)
	{
		RenderCurr();
		m_textureID = textureID;
		m_color = color;
		m_alpha = alpha;
		m_transparent = transparent;
		m_smooth = smooth;
	}
}	// Render

// render the currently stored quads
void GLCallBatcher::RenderCurr()
{
	if (m_numQuads == 0) return;


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	const float inv256 = 1.0f/256.0f;
	float red = ((m_color>>16)&0xff)*inv256;
	float green = ((m_color>>8)&0xff)*inv256;
	float blue = (m_color&0xff)*inv256;
	if (m_alpha > 1.0f) m_alpha = 1.0f;
	glColor4f(red, green, blue, m_alpha);
#ifdef _DEBUG
	int err = glGetError();
	if (err != GL_NO_ERROR)
	{
		cout << "OpenGL error!!!2 " << err << endl;
	}
#endif
	glVertexPointer(3, GL_FLOAT, sizeof(float)*dataPerVextex, m_pVertexData); 
	glTexCoordPointer(compVertTex, GL_FLOAT, sizeof(float)*dataPerVextex, 
					  &m_pVertexData[3]);
	assert(m_numQuads*indicesPerQuad < 0xffff);
    glDrawElements(GL_TRIANGLES, (GLsizei)(m_numQuads*indicesPerQuad), 
		GL_UNSIGNED_SHORT, m_pIndices);
	m_drawCallCount++;
	m_quadCount += m_numQuads;
	m_numQuads = 0;

#ifdef _DEBUG
	 err = glGetError();
	if (err != GL_NO_ERROR)
	{
		cout << "OpenGL error!!! " << err << endl;
	}
#endif
	

}

// increase the memory alloacation
// new memory is allocated and old memory is copied, then old memory is deleted
void GLCallBatcher::Reallocate(int increase)
{
	int newSize = m_numQuads+increase;
	float* pVertices = new float[newSize*verticesPerQuad*dataPerVextex];	// x,y,z
	memset(pVertices, 0, newSize*verticesPerQuad*dataPerVextex*sizeof(float));
	GLushort* pIndices = new GLushort[newSize*indicesPerQuad];   // 2 triangles per quad
	// indices never change
	for (int n = 0; n < newSize; n++)
	{
		pIndices[n*indicesPerQuad] = (GLshort)n*verticesPerQuad;
		pIndices[n*indicesPerQuad+1] = (GLshort)n*verticesPerQuad+1;
		pIndices[n*indicesPerQuad+2] = (GLshort)n*verticesPerQuad+2;
		pIndices[n*indicesPerQuad+3] = (GLshort)n*verticesPerQuad+1;
		pIndices[n*indicesPerQuad+4] = (GLshort)n*verticesPerQuad+3;
		pIndices[n*indicesPerQuad+5] = (GLshort)n*verticesPerQuad+2;
	}
	if (m_numQuads > 0)
	{
		int numVert = m_numQuads*verticesPerQuad;
		memcpy(pVertices, m_pVertexData, numVert*dataPerVextex*sizeof(float));
//		memcpy(pTexCoords, m_pTexCoords, numVert*compVertTex*sizeof(float));
		Release();
	}
	m_pVertexData = pVertices;
//	m_pTexCoords = pTexCoords;
	m_pIndices = pIndices;
	m_sizeCache = newSize;
}

// output the statistics
void GLCallBatcher::OutputData(ostringstream& os) 
{
	os << " draw calls: " << m_drawCallCount << " quads:" << m_quadCount;
}
