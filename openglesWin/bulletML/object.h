#ifndef object_h_
#define object_h_

enum ObjectType{
	COMMON,
	BOSS,
	PLAYER
};

class Object {
protected:
    Object(double x, double y, double d, double s, unsigned int c, ObjectType);

public:
    virtual ~Object();

    virtual bool move();
    virtual bool draw();

    double x, y;
    double d, s;
    unsigned int col;
	ObjectType type;

	int anim;

};

class Shot : public Object {
public:
    Shot(double x, double y, double d, double s,ObjectType type);
	~Shot();
};

class Bullet : public Object {
public:
    Bullet(class BulletMLState* state, double x, double y, double d, double s,ObjectType type);
    Bullet(class BulletMLParser* parser,
           double x, double y, double d, double s,ObjectType type);
	~Bullet();

    virtual bool move();
    virtual bool draw();
    void die() { alive_ = false; }
	bool invisible;

private:
    class BulletCommand* command_;
    bool alive_;
	
};
#endif // ! object_h_

