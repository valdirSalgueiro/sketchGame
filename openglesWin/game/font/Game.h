#ifndef _GAME_H_INCLUDED_
#define _GAME_H_INCLUDED_


class Game
{
public:
	Game();
	~Game();
	void Init(const string& dirResources, const string& dirUser, 
		int screenWidth, int screenHeight);
	void Render();
	// used to display error on screen
	const string& GetErr() const
	{
		return m_strErr;
	}
	void Update();
private:
	int m_timeLast;
	int m_timeStart;
	string m_strErr;
};





#endif // _GAME_H_INCLUDED_
