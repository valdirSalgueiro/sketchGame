#ifndef _GLCALLBATCHER_H_INCLUDED_
#define _GLCALLBATCHER_H_INCLUDED_

// AtlasImage objects send their quads to GLCallBatcher for rendering.
// GLCallBatcher strings the quads together into triangle lists.
// All quads with same alpha, colour, transpancy and texture are rendered together 
// with one call to glDrawElements
class GLCallBatcher
{
public:
	GLCallBatcher();
	~GLCallBatcher();
	void AddQuad(GLuint textureID, int color, float alpha, bool transparent, bool smooth,
		const float* pTexCoords, const float* pVertices);
	void RenderCurr();
	void OutputData(ostringstream& os);
	void ResetStats()
	{
		m_drawCallCount = 0;
		m_quadCount = 0;
	}
	void SetAttribs(GLuint textureID, int color, float alpha, bool transparent, bool smooth);
	void AddQuad(const float* pTexCoords, const float* pVertices);
	void Release();
private:
	void Reallocate(int increase);
	float* m_pVertexData;		// storage of vertex array
	GLushort* m_pIndices;	// indices of quads
	int m_numQuads;			// current number of quads waiting to be rendered
	int m_sizeCache;		// current amount of quads that can fit in the arrays
	float m_alpha;			// alpha value of current quads
	int m_color;			// colour tint of current quads
	GLuint m_textureID;		// texture of current quads
	bool m_transparent;		// are current quads transparent
	int m_drawCallCount;	// statistical info
	int m_quadCount;
	bool m_smooth;
};





#endif // _GLCALLBATCHER_H_INCLUDED_
