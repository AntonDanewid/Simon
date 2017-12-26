//Created by Anton Danewid
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sstream>
#include <list>
#include <cmath>
#include <sys/time.h>
#include "Simon.h"

using namespace std;
Display* display;
Window window;
XEvent event;
int score = 0;
int screen;
int w = 800;
int h = 400;
int FPS = 60;
unsigned long end = 0;
unsigned long lastRepaint = 0;




struct XInfo {
	Display*  display;
	Window   window;
	GC       gc;
};

class Displayable {
public:
	virtual void paint(XInfo& xinfo) = 0;
	virtual void move(int a, int b) = 0;
};

class Text : public Displayable {
public:
	virtual void paint(XInfo& xinfo) {
		XDrawImageString(xinfo.display, xinfo.window, xinfo.gc,
			this->x, this->y, this->s.c_str(), this->s.length());
		//cout << "here" << endl;
	}

	virtual void move(int a, int b) {
		x = x + a;
		y = y + b;
	}

	// constructor
	Text(int x, int y, string s) : x(x), y(y), s(s) {}



	void changeText(string a) {
		s = a;
	}

private:
	int x;
	int y;
	string s; // string to show
};


class Circle : public Displayable {
public:
	virtual void paint(XInfo& xinfo) {
		XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 50, this->y - 50, 100, 100, 0, 360 * 64);
		XDrawImageString(xinfo.display, xinfo.window, xinfo.gc,
			this->x-5, this->y+10, this->name.c_str(), this->name.length());
		if (hit) {
			XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 99 / 2, this->y - 99 / 2, 99, 99, 0, 360 * 64);
			XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 98 / 2, this->y - 98 / 2, 98, 98, 0, 360 * 64);
			XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 97 / 2, this->y - 97 / 2, 97, 97, 0, 360 * 64);
		}


	}

	void highLight(XInfo& xinfo) {
		XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 99 / 2, this->y - 99 / 2, 99, 99, 0, 360 * 64);
		XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 98 / 2, this->y - 98 / 2, 98, 98, 0, 360 * 64);
		XDrawArc(xinfo.display, xinfo.window, xinfo.gc, this->x - 97 / 2, this->y - 97 / 2, 97, 97, 0, 360 * 64);
	}

	virtual void move(int a, int b) {

		x = a;
		y = b;

	}



	Circle(int a, int b, int ID) {
		x = a;
		y = b;
		iD = ID;
		hit = false;
		stringstream ss;
		ss << ID + 1;
		name = ss.str();
	}

	int getNum() {
		return iD;
	}

	int getX() {
		return x;
	}

	int getY() {
		return y;
	}

	bool hitDet(int a, int b) {

		int centerX = x;
		int centerY = y;

		if ((centerX - a)*(centerX - a) + (centerY - b) *  (centerY - b) < 50 * 50) {
			hit = true;
			return true;

		}
		else {
			hit = false;
			return false;
		}





	}




private:

	string name;
	int iD;
	bool hit;
	int x;
	int y;
};


//Function definitions
void initDisplay(XInfo& xinfo);
void repaint(list<Displayable*> dList, XInfo& xinfo);
void moveList(list<Circle*> dList, int a, int b, int n);
int collision(list<Circle*> circleList, int a, int b, XInfo& xinfo);
void playGameRound(Simon simon, Circle* cList[], XInfo& xinfo, list<Displayable*> dList);
void animate(Circle* c, XInfo& xinfo, list<Displayable*> dList);
int calcCircleX(int x, int num, int tot);
int calcCircleY(int y);
unsigned long now();

unsigned long now();



int main(int argc, char* argv[]) {




	//cout << "ARGS ARE " << argc << " " <<  argv[1] << endl;
	int n = 0;

	if (argc < 2) {
		cout << "Too few arguments";
		exit(-1);
	}
	else {
		char temp = argv[1][0];
		n = temp - '0';


	}


	XInfo xinfo;

	//Add all displayable objects to lists
	initDisplay(xinfo);
	Text* messageText = new Text(30, 100, "Press SPACE to play");
	Text* score = new Text(30, 30, "0");
	score->paint(xinfo);
	messageText->paint(xinfo);

	list<Displayable*> displayList;
	list<Circle*> buttons;


	list<Displayable*> movable;


	displayList.push_back(messageText);
	displayList.push_back(score);

	Circle* cList[n];


	for (int i = 0; i < n; i++) {
		Circle* c = new Circle(calcCircleX(w, i, n), calcCircleY(h), i);
		cList[i] = c;
		displayList.push_back(c);movable.push_back(c);
		buttons.push_back(c);
	}

	repaint(displayList, xinfo);
	Simon simon = Simon(n, true);

	int mover = 1;


	while (true) {

		// leave some space between rounds
		//usleep(1000000 / FPS - (now() - lastRepaint));
		//end = now();
		//cout << "state is " << simon.getStateAsString() << endl;

		

		if (simon.getState() == Simon::START) {
			int counter = 0;

			while (XPending(xinfo.display) == 0) {
				if (cList[0]->getY() >= h/2 +40 || cList[0]->getY() <= h / 2 - 40) {
					mover = -mover;
					counter = 0;
				}
				for (int i = 0; i < n; i++) {
					cList[i]->move(cList[i]->getX(), cList[i]->getY() + mover);
					repaint(displayList, xinfo);
					usleep(10000);

					counter++; 
				}
			}
			


		}



		if (simon.getState() == Simon::WIN) {
			messageText->changeText("You win! Press SPACE to continue");
			stringstream ss;
			ss << simon.getScore();
			score->changeText(ss.str());
			repaint(displayList, xinfo);

		}


		if (simon.getState() == Simon::LOSE) {
			messageText->changeText("You lost! Press SPACE to continue");
			stringstream ss;
			ss << 0;
			score->changeText(ss.str());
			repaint(displayList, xinfo);
			repaint(displayList, xinfo);

		}

		XNextEvent(xinfo.display, &event);
		long start = now();
		switch (event.type) {

		case KeyPress:

			if (event.xkey.keycode == 24) {
				cout << event.xkey.keycode << endl;
				exit(0);
			}
			else if (event.xkey.keycode == 65) {
				moveList(buttons, w, h, n);
				repaint(displayList, xinfo);


				simon.newRound();
				stringstream ss;
				ss << simon.getScore();
				score->changeText(ss.str());
				messageText->changeText("Watch what I do");
				repaint(displayList, xinfo);
				while (simon.getState() == Simon::COMPUTER) {
					int i = simon.nextButton();
					animate(cList[i], xinfo, displayList);
					XFlush(xinfo.display);
					usleep(500000);
					repaint(displayList, xinfo);
					XFlush(xinfo.display);
					usleep(250000);

				}
				messageText->changeText("Your turn");
				repaint(displayList, xinfo);

			}
			break;



		case ButtonPress:
			if (simon.getState() == Simon::HUMAN) {
				int a = collision(buttons, event.xbutton.x, event.xbutton.y, xinfo);
				animate(cList[a], xinfo, displayList);
				if (a != 0) {

					simon.verifyButton(a);
					cout << "Clicked" << endl;
				}
			}
			break;




		case ConfigureNotify:
			w = event.xconfigure.width;
			h = event.xconfigure.height;
			moveList(buttons, w, h, n);
			cout << w << endl;
			repaint(displayList, xinfo);
			break;
			




		case MotionNotify:
			int x = event.xmotion.x;
			int y = event.xmotion.y;
			collision(buttons, x, y, xinfo);
			repaint(displayList, xinfo);
			break;
		}

		//usleep(1000000 / FPS - (start - now()));
	}

}



/*Calculates a circles alligment in relation to the window dimensions*/
int calcCircleX(int x, int num, int tot) {
	return (num*x) / tot + x / (2 * tot);
}
/*Calculates a circles alligment in relation to the window dimensions*/

int calcCircleY(int y) {
	return y / 2;
}

/*Inits a display and a graphics context*/

void initDisplay(XInfo& xinfo) {

	xinfo.display = XOpenDisplay("");
	if (!xinfo.display) exit(-1);
	screen = DefaultScreen(xinfo.display);

	xinfo.window = XCreateSimpleWindow(xinfo.display, DefaultRootWindow(xinfo.display),
		10, 10, w, h, 2,
		BlackPixel(xinfo.display, screen), WhitePixel(xinfo.display, screen));
	XMapRaised(xinfo.display, xinfo.window);
	XFlush(xinfo.display);
	XSelectInput(xinfo.display, xinfo.window, PointerMotionMask | KeyPressMask | StructureNotifyMask | ButtonPressMask);


	// give server 10ms to get set up before sending drawing commands
	usleep(10 * 1000);
	// drawing demo with graphics context here ...
	GC gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	xinfo.gc = gc;		// create a graphics context
	XSetForeground(xinfo.display, xinfo.gc, BlackPixel(xinfo.display, screen));
	XSetBackground(xinfo.display, xinfo.gc, WhitePixel(xinfo.display, screen));
	XFontStruct * font;
	font = XLoadQueryFont(xinfo.display, "12x24");
	XSetFont(xinfo.display, xinfo.gc, font->fid);
	XFlush(xinfo.display);


}
/*Animates a cicle */
void animate(Circle* c, XInfo& xinfo, list<Displayable*> dList) {
	int x = c->getX();
	int y = c->getY();
	GC gc = XCreateGC(xinfo.display, xinfo.window, 0, 0);
	XSetBackground(xinfo.display, gc, WhitePixel(xinfo.display, screen));
	XSetForeground(xinfo.display, gc, WhitePixel(xinfo.display, screen));
	
	//end = 0; 
	//last repaint = 0; 
	
	for (int i = 0; i < 30; i++) {
		end = now();


		XFillArc(xinfo.display, xinfo.window, xinfo.gc, x - 50, y - 50, 100, 100, 0, 360 * 64);
		XDrawArc(xinfo.display, xinfo.window, gc, x - (100 - i * 3) / 2, y - (100 - i * 3) / 2, 100 - i * 3, 100 - i * 3, 0, 360 * 64);
		XFlush(xinfo.display);
		usleep(25000);
		
		//usleep(1000000 / FPS - (end - lastRepaint));

		
		repaint(dList, xinfo);
		//lastRepaint = now();
	}


}

/*Repaints a list of displayables*/
void repaint(list<Displayable*> dList, XInfo& xinfo) {
	list<Displayable*>::const_iterator start = dList.begin();
	list<Displayable*>::const_iterator end = dList.end();
	XClearWindow(xinfo.display, xinfo.window);
	while (start != end) {
		Displayable* d = *start;
		d->paint(xinfo);
		start++;
	}
	XFlush(xinfo.display);
}

/*Moves a list of circles, not displayables, as theese are the only ones that move around*/
void moveList(list<Circle*> dList, int a, int b, int n) {
	list<Circle*>::const_iterator start = dList.begin();
	list<Circle*>::const_iterator end = dList.end();
	while (start != end) {
		Circle* d = *start;
		d->move(calcCircleX(a, d->getNum(), n), calcCircleY(b));
		start++;
	}
}

/*Checks if a x and y coordinate is inside a circle, returns the ID of the circle if collision occurs, 0 otherwise*/
int collision(list<Circle*> circleList, int a, int b, XInfo& xinfo) {
	list<Circle*>::const_iterator start = circleList.begin();
	list<Circle*>::const_iterator end = circleList.end();
	while (start != end) {
		Circle* c = *start;
		if (c->hitDet(a, b)) {
			c->highLight(xinfo);
			return c->getNum();
		}
		start++;
	}
	return 0;
}


unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}
