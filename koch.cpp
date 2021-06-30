#include <SFML\Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
using namespace sf;
using namespace std;

class Complex {
private:
	float Re, Im;
public:
	Complex(float Re, float Im) : Re(Re), Im(Im) {}
	Complex operator+(const Complex &z) { return Complex(Re + z.getRe(), Im + z.getIm()); }
	Complex operator-(const Complex &z) { return Complex(Re - z.getRe(), Im - z.getIm()); }
	Complex operator*(const Complex &z) { return Complex(Re * z.getRe() - Im * z.getIm(), Re * z.getIm() + Im * z.getRe()); }
	Complex operator*(float k) { return Complex(Re * k, Im * k); } // scalar multiplication
	Complex operator/(float k) { return Complex(Re / k, Im / k); } // scalar division
	Vertex vertex(Color col) { return Vertex(Vector2f(Re, Im), col); }
	float getRe() const { return Re; }
	float getIm() const{ return Im; }
	void print() { cout << Re << " + (" << Im << ")i"; }
};

const Complex ω(- 0.5f, 0.5f * sqrtf(3));

const int W = 1920, H = 1080;

unsigned long long numTriangles = 3;

float side, height;

queue<pair<Complex, Complex>> Q;

Complex divideSegmentInRatio(Complex A, Complex B, float k) { return (A + (B * k)) / (1 + k); } // For 2 points A and B, find the point M that divides [AB] in ratio k. MA/MB = k.

Complex equilateralTrianglePoint(Complex A, Complex B) { return A + ((A - B) * ω); } // For 2 points A and B, find the third point C such that △ABC is equilateral.

void drawLine(Complex A, Complex B, Color col, RenderWindow& win) 
{
	Vertex line[] = {A.vertex(col), B.vertex(col)};
	win.draw(line, 2, Lines);
}

void drawSemiTriangle(Complex A, Complex B, Complex C, void (*lineDrawingFunc)(Complex, Complex, Color, RenderWindow&), RenderWindow& win) 
{
	lineDrawingFunc(A, B, Color::Black, win);
	lineDrawingFunc(A, C, Color::White, win);
	lineDrawingFunc(B, C, Color::White, win);
}

void drawFullTriangle(Complex A, Complex B, Complex C, void (*lineDrawingFunc)(Complex, Complex, Color, RenderWindow&), RenderWindow& win) 
{
	lineDrawingFunc(A, B, Color::White, win);
	lineDrawingFunc(A, C, Color::White, win);
	lineDrawingFunc(B, C, Color::White, win);
}

int main() {
	cout << "Triangle side = "; cin >> side;

	height = 0.5f * sqrtf(3) * side;

	Complex A(0.5f * (W - side), height + 0.5f * (H - height));
	Complex B(W - 0.5f * (W - side), height + 0.5f * (H - height));
	Complex C = equilateralTrianglePoint(A, B);

	Q.push({A, C});
	Q.push({C, B});
	Q.push({B, A});

	RenderWindow win(VideoMode(W, H), "Koch snowflake fractal");

	win.setKeyRepeatEnabled(false);

	drawFullTriangle(A, B, C, drawLine, win);

	while(win.isOpen()) {
		win.display();

		Event E;
		while(win.pollEvent(E)) {
			if(E.type == Event::Closed) {
				win.close();
			}
			if(E.type == Event::KeyPressed && E.key.code == Keyboard::Enter) {

				// 1 line - 1 drawn triangle + 4 future triangles

				for(unsigned long long i = 0; i < numTriangles; i++) {
					pair<Complex, Complex>& lineCurr = Q.front();
					Complex A = divideSegmentInRatio(lineCurr.first, lineCurr.second, 0.5f);
					Complex B = divideSegmentInRatio(lineCurr.second, lineCurr.first, 0.5f);
					Complex C = equilateralTrianglePoint(A, B);
					drawSemiTriangle(A, B, C, drawLine, win);
					vector<pair<Complex, Complex>> nextGeneration
					{
						{lineCurr.first, A},
						{A, C},
						{C, B},
						{B, lineCurr.second},
					};
					for(pair<Complex, Complex>& line : nextGeneration) {
						Q.push(line);
					}
					Q.pop();
				}
				numTriangles *= 4;
			}
		}
		
	}
	return 0;
}