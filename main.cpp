#include "lib/framework.hpp"
#include <list>
#include <ctime>

enum Size {
	WIDTH = 1000,
	HEIGHT = 1000
};

int PinB(float x, float y, float w, float h, float px, float py) {
	if (px > x && px < x + w &&
		py > y && py < y + h)
		return 1;
	return 0;
}

Vec2f whCalc(int n) {
	float w = ((n - 1) % 5)*(96.5);
	int h = 0;
	if (n > 5) h = 128;
	return Vec2f(w, h);
}

void eraseNum(std::list<int>& numList, int delNum) {
	for (auto it = numList.begin(); it != numList.end(); ++it) {
		if (*it == delNum) {
			numList.erase(it);
			return;
		}
	}
}

void problemCreation(int correct[9][9],int selectedNum,int num[9][9],int hidden[9][9],int count[9]) {
	Random dice;
	dice.setSeed(std::time(nullptr));
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			correct[i][j] = 1;
			int rand = dice(4);
			if (rand == 0) {
				correct[i][j] = 0;
				count[num[i][j] - 1] -= 1;
			}
		}
	}
	for (int j = 0; j < 9; j++) {
		for (int k = 0; k < 9; k++) {
			if (selectedNum == num[j][k]) {
				for (int l = 0; l < 3; l++) {
					for (int m = 0; m < 3; m++) {
						if (correct[j][k] == 1) {
							hidden[(j - (j % 3)) + l][k - (k % 3) + m] = 1;
						}
					}
				}
			}
		}
	}
}

int main() {
	AppEnv env(Size::WIDTH, Size::HEIGHT);
	int isGameLooping = true;
	Random dice;
	dice.setSeed(std::time(nullptr));
	while (isGameLooping) {

		Font font("res/ObliviousFont.ttf");

		Texture toten("res/onetoten.png");
		Texture loading("res/NowLoading.jpg");
		env.begin();
		drawTextureBox(-WIDTH / 2, -HEIGHT / 2, WIDTH, HEIGHT, 0, 0, 512, 512, loading);
		env.end();
		const int edgeLength = 85;
		const int shiftx = 120;
		const int shifty = -100;
		int num[9][9];
		int correct[9][9];
		int hidden[9][9];
		std::list<int> possibility[9][9];

		int correctCount[9] = { 9,9,9,9,9,9,9,9,9 };
		int selectedNum = 1;
		int isMissed = 0;
		float quakeRange = 1;
		bool isCleared = false;

		float alpha = 1;
		Color texColor = Color(1, 1, 1, 1);
		Color boxColor = Color(1, 1, 1, 1);
		Color numColor = Color(1, 1, 1, 1);

		int timer = 0;
		int clearTime = 0;
		Vec2f tsize = font.drawSize("00:00.0");

		/*generator*/ {
		INIT:
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					for (int k = 0; k < 9; k++) {
						possibility[i][j].clear();
					}
				}
			}

			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					for (int k = 0; k < 9; k++) {
						possibility[i][j].push_back(k + 1);
					}
				}
			}

			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {

					if (possibility[i][j].size() == 0) {
						printf("やっべ!");
						goto INIT;
					}

					int rand = dice(possibility[i][j].size());
					auto it = possibility[i][j].begin();

					for (int k = 0; k < rand; k++) {
						++it;
					}

					int Decision = *it;
					num[i][j] = Decision;

					for (int k = 0; k < 9; k++) {
						eraseNum(possibility[i][k], Decision);
						eraseNum(possibility[k][j], Decision);
					}

					for (int k = 0; k < 3; k++) {
						for (int l = 0; l < 3; l++) {
							eraseNum(possibility[(i - (i % 3)) + k][j - (j % 3) + l], Decision);
						}
					}
				}
			}
		}
		problemCreation(correct, selectedNum, num, hidden, correctCount);
		font.size(60);

	while (env.isOpen()) {
		env.begin();
		Vec2f mouse = env.mousePosition();

		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				if (selectedNum == num[j][k]) {
					for (int l = 0; l < 3; l++) {
						for (int m = 0; m < 3; m++) {
							if (correct[j][k] == 1) {
								hidden[(j - (j % 3)) + l][k - (k % 3) + m] = 1;
							}
						}
					}
				}
			}
		}



		{ // draw
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					Vec2f wh = whCalc(num[i][j]);
					if (hidden[i][j] == 1) {
						texColor = Color(0.5, 0.5, 0.5, 1);
						boxColor = Color(0.5, 0.5, 0.5, 1);
						if (selectedNum == num[i][j]) {
							texColor = Color(1, 1, 0, 1);
						}
					}
					else {
						if (PinB(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, mouse.x(), mouse.y())) {

							texColor = Color(0, 1, 1, 1);
							boxColor = Color(0, 1, 1, 1);
						}
						else {
							texColor = Color(1, 1, 1);
							boxColor = Color(1, 1, 1);
						}
					}
					if (correct[i][j] == 1) {
						drawTextureBox(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, wh.x(), wh.y(), 102.4, 128,
							toten, texColor);
					}
					else {
						drawFillBox(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, boxColor);
					}

					/*line*/ {
						drawBox(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, 1, Color(0, 0, 0));
						if ((i + 1) % 3 == 0 && j % 3 == 0) {
							drawBox(j*edgeLength - WIDTH / 2 + shiftx,
								HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
								edgeLength * 3, edgeLength * 3, 5, Color(0, 0, 0));
						}
					}
				}
				Vec2f wh = whCalc(i + 1);
				if (PinB(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, mouse.x(), mouse.y())) {
					numColor = Color(0, 1, 1, 1);
				}
				else {
					numColor = Color(1, 1, 1);
				}
				if (selectedNum == i + 1) {
					numColor = Color(1, 1, 0, 1);
				}
				if (correctCount[i] == 9) {
					numColor = Color(0.5, 0.5, 0.5, 1);
				}
				drawTextureBox(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, wh.x(), wh.y(), 102.4, 128,
					toten, numColor);
				drawBox(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, 1, Color(0, 0, 0));
			}
		}
		font.draw("00:00.0",
			Vec2f(-WIDTH / 2 + shiftx+200, HEIGHT / 2 + shifty),
			Color(1, 1, 1));

		font.draw("TIME:",
			Vec2f(-WIDTH / 2 + shiftx, HEIGHT / 2 + shifty),
			Color(1, 1, 1));
		drawFillBox(-WIDTH / 2, -HEIGHT / 2, WIDTH, HEIGHT, 
			Color(0, 0, 0, alpha));
		Vec2f s = font.drawSize("GAME START");
		font.draw("GAME START",
			Vec2f(-s.x()/2,0),Color(1,0,0,alpha));
		alpha -= 0.005F;
		if (alpha < 0.1) break;
		env.end();
	}

	while (env.isOpen()) {
		env.begin();

		timer++;

		Vec2f mouse = env.mousePosition();

		for (int i = 0; i < 9; i++) {
			if (env.isButtonPushed(Mouse::LEFT) &&
				PinB(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, mouse.x(), mouse.y())) {

				for (int j = 0; j < 9; j++) {
					for (int k = 0; k < 9; k++) {
						hidden[j][k] = 0;
					}
				}
				selectedNum = i + 1;
				break;
			}
			if (env.isKeyPushed('0'+(i+1))) {
				for (int j = 0; j < 9; j++) {
					for (int k = 0; k < 9; k++) {
						hidden[j][k] = 0;
					}
				}
				selectedNum = i + 1;
				break;
			}
		}

		for (int j = 0; j < 9; j++) {
			for (int k = 0; k < 9; k++) {
				if (selectedNum == num[j][k]) {
					for (int l = 0; l < 3; l++) {
						for (int m = 0; m < 3; m++) {
							if (correct[j][k] == 1) {
								hidden[(j - (j % 3)) + l][k - (k % 3) + m] = 1;
							}
						}
					}
				}
			}
		}

		/* draw*/{
			for (int i = 0; i < 9; i++) {
				for (int j = 0; j < 9; j++) {
					Vec2f wh = whCalc(num[i][j]);
					if (hidden[i][j] == 1) {
						texColor = Color(0.5,0.5,0.5,1);
						boxColor = Color(0.5, 0.5, 0.5, 1);
						if (selectedNum == num[i][j]) {
							texColor = Color(1,1,0,1);
						}
					}
					else {
						if (PinB(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, mouse.x(), mouse.y())) {

							texColor = Color(0, 1, 1, 1);
							boxColor = Color(0,1,1,1);
						}
						else {
							texColor = Color(1, 1, 1);
							boxColor = Color(1,1,1);
						}
					}
					if (correct[i][j] == 1) {
						drawTextureBox(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, wh.x(), wh.y(), 102.4, 128,
							toten, texColor);
					}
					else {
						drawFillBox(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, boxColor);
					}

					/*line*/{
						drawBox(j*edgeLength - WIDTH / 2 + shiftx,
							HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
							edgeLength, edgeLength, 1, Color(0, 0, 0));
						if ((i + 1) % 3 == 0 && j % 3 == 0) {
							drawBox(j*edgeLength - WIDTH / 2 + shiftx,
								HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
								edgeLength * 3, edgeLength * 3, 5, Color(0, 0, 0));
						}
					}
				}
				Vec2f wh = whCalc(i + 1);
				if (PinB(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, mouse.x(), mouse.y())) {
					numColor = Color(0,1,1,1);
				}
				else {
					numColor = Color(1,1,1);
				}
				if (selectedNum == i + 1) {
					numColor = Color(1,1,0,1);
				}
				if (correctCount[i] == 9) {
					numColor = Color(0.5, 0.5, 0.5, 1);
				}
				drawTextureBox(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, wh.x(), wh.y(), 102.4, 128,
					toten,numColor);
				drawBox(i*edgeLength - WIDTH / 2 + shiftx,
					-HEIGHT / 2,
					edgeLength, edgeLength, 1, Color(0, 0,0));
			}
			{
				font.draw("TIME:",
					Vec2f(-WIDTH / 2 + shiftx, HEIGHT / 2 + shifty),
					Color(1, 1, 1));

				int a = timer % 60;
				float rate = a / 60.0F;
				int mirisec = 10 * rate;
				int sec = (timer / 60) % 60;
				int min = timer / 3600;

				char text[100];
				sprintf(text, "%02d:%02d.%d",
					min, sec, mirisec);
				font.draw(text,
					Vec2f(-WIDTH / 2 + shiftx + 200, HEIGHT / 2 + shifty),
					Color(1, 1, 1));
			}
			if (isMissed) {
				quakeRange -= 0.1F;
				if (quakeRange < 0) {
					quakeRange = 1;
					isMissed = 0;
				}
				float randx = dice(10);
				float randy = dice(10);
				Vec2f size = font.drawSize("miss");
				font.draw("miss",
					Vec2f(mouse.x()-size.x()/2+randx, mouse.y()+randy), Color(1, 0, 0));
			}
		}

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (env.isButtonPushed(Mouse::LEFT) &&
					PinB(j*edgeLength - WIDTH / 2 + shiftx,
						HEIGHT / 2 - i * edgeLength - edgeLength + shifty,
						edgeLength, edgeLength, mouse.x(), mouse.y())) {

					if (num[i][j] == selectedNum) {
						if (correct[i][j] == 0)
							correctCount[selectedNum - 1] ++;

						correct[i][j] = 1;

						if (correctCount[0] == 9 &&
							correctCount[1] == 9 &&
							correctCount[2] == 9 &&
							correctCount[3] == 9 &&
							correctCount[4] == 9 &&
							correctCount[5] == 9 &&
							correctCount[6] == 9 &&
							correctCount[7] == 9 &&
							correctCount[8] == 9) {
							clearTime = timer;
							isCleared = true;
						}
					}
					else {
						if (correct[i][j] == 0) {
							isMissed = 1;
							timer += 3600;
						}
						else {
							isMissed = 0;
						}
					}
				}
			}
		}
		if (isCleared) break;
		env.end();
	}
	if (isCleared == false) break;
	Vec2f size = font.drawSize("CLEAR!");
	font.size(100);

	while (env.isOpen()) {
		env.begin();

		Vec2f mouse = env.mousePosition();

		{
			size = font.drawSize("TIME: 00:00.0");
			font.draw("TIME:",
				Vec2f(-size.x()/2, -50),
				Color(1, 1, 1));

			int a = clearTime % 60;
			float rate = a / 60.0F;
			int mirisec = 10 * rate;
			int sec = (clearTime / 60) % 60;
			int min = clearTime / 3600;

			char text[100];
			sprintf(text, "%02d:%02d.%d",
				min, sec, mirisec);
			font.draw(text,
				Vec2f(-30, -50),
				Color(1, 1, 1));
		}

		font.size(100);
		size = font.drawSize("CLEAR!");
		font.draw("CLEAR!",
			Vec2f(-size.x()/2,70),Color(1,1,0));

		font.size(60);
		size = font.drawSize("Restart");
		font.draw("Restart",
			Vec2f(-size.x() / 2 - 100, -140), Color(0, 1, 0.5));

		if (PinB(-size.x() / 2-100, -140, size.x(), size.y(), mouse.x(), mouse.y()) &&
			env.isButtonPushed(Mouse::LEFT)) {
			break;
		}
		size = font.drawSize("Quit");
		font.draw("Quit",
			Vec2f(-size.x() / 2 + 160, -140), Color(0, 0.5, 1));

		if (PinB(-size.x() / 2 + 160, -140, size.x(), size.y(), mouse.x(), mouse.y()) &&
			env.isButtonPushed(Mouse::LEFT)) {
			isGameLooping = false;
			break;
		}

		env.end();
	}
	}
}
