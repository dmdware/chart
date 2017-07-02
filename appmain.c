
#include "sys/includes.h"
#include "sys/texture.h"

struct v2f
{
	float x, y;
};

typedef struct v2f v2f;

struct mass2f
{
	v2f p;
	float gm;
};

typedef struct mass2f mass2f;

v2f perp(v2f slope)
{
	v2f nextslope;
	float magf, dx, dy;
	nextslope.x = slope.x;
	nextslope.y = slope.y;

#if 01
	if (slope.x > 0 &&
		slope.y > 0)
		nextslope.y *= -1;
	else if (slope.x > 0 &&
		slope.y == 0)
		nextslope.x *= -1;
	else if (slope.x > 0 &&
		slope.y < 0)
		nextslope.x *= -1;
	else if (slope.x == 0 &&
		slope.y < 0)
		;
	else if (slope.x < 0 &&
		slope.y < 0)
		nextslope.y *= -1;
	else if (slope.x < 0 &&
		slope.y == 0)
		nextslope.y *= -1;
	else if (slope.x < 0 &&
		slope.y > 0)
		nextslope.y *= -1;
	else if (slope.x == 0 &&
		slope.y > 0)
		;
#endif

	//nextslope.y *= -1;

	dx = nextslope.x;
	dy = nextslope.y;
	magf = sqrtf(dx*dx + dy*dy);

	if (magf < 0.00001f)
		return nextslope;

	nextslope.x /= magf;
	nextslope.y /= magf;

	return nextslope;
}

void main()
{
	v2f start;
	v2f startdir;
	v2f altdir;
	v2f currpt;
	v2f curraccel;
//	v2f nextdir;
	int numexpand = 30;
	int numpts = 90;
	int expansioni;
	int startpti;
	int startptj;
	int massi;
	float dx, dy;
	v2f perpdir;
	char foo[123];
	FILE *fp;
	float linei;
	v2f nextpt;
	float magf;
	unsigned char temprgb[3];

#define SPACING	1.0f

#define RGBX	512
#define RGBY	512
#define MINX	(-10)
#define MINY	(-10)
#define SCALE	20

	unsigned char rgb[3 * RGBX * RGBY];
	int x, y;

#define MASSES 5

	mass2f masses[MASSES];

	masses[0].gm = 1;
	masses[1].gm = 2;
	masses[2].gm = 3;
	masses[3].gm = 4;
	masses[4].gm = 5;

	masses[0].p.x = 0;
	masses[0].p.y = 0;
	masses[1].p.x = -1;
	masses[1].p.y = 4;
	masses[2].p.x = 8;
	masses[2].p.y = 5.1f;
	masses[3].p.x = 11;
	masses[3].p.y = -2.7f;
	masses[4].p.x = -7.9;
	masses[4].p.y = -8.9f;

	start.x = -4;
	start.y = 11.2f;
	startdir.x = 1 / SPACING;
	startdir.y = -0.1f / SPACING;
	altdir = perp(startdir);
	altdir.x /= SPACING;
	altdir.y /= SPACING;

	//return;

	for (x = 0; x < RGBX; ++x)
	{
		for (y = 0; y < RGBY; ++y)
		{
			rgb[3 * (x + y*RGBX) + 0] = 255;
			rgb[3 * (x + y*RGBX) + 1] = 255;
			rgb[3 * (x + y*RGBX) + 2] = 255;
		}
	}

	fp = fopen("out.txt", "w");
	//return;

	for (startptj = 0; startptj < numpts; startptj++)
	{
		for (startpti = 0; startpti < numpts; startpti++)
		{
			currpt.x = start.x + startdir.x * startpti + altdir.x * startptj;
			currpt.y = start.y + startdir.y * startpti + altdir.y * startptj;

			for (expansioni = 0; expansioni < numexpand; expansioni++)
			{
				fprintf(fp, "startpt%d,expansion%d: pt%f,%f\r\n", startpti, expansioni, currpt.x, currpt.y);

				if (currpt.x * SCALE - MINX * SCALE < 0)
					break;
				if (currpt.y * SCALE - MINY * SCALE < 0)
					break;
				if (currpt.x * SCALE - MINX * SCALE >= RGBX)
					break;
				if (currpt.y * SCALE - MINY * SCALE >= RGBY)
					break;

				curraccel.x = 0;
				curraccel.y = 0;

				for (massi = 0; massi < MASSES; massi++)
				{
					dx = (masses[massi].p.x - currpt.x);
					dy = (masses[massi].p.y - currpt.y);
					curraccel.x += dx * masses[massi].gm / (dx*dx + dy*dy);
					curraccel.y += dy * masses[massi].gm / (dx*dx + dy*dy);
				}

				perpdir = perp(curraccel);

				dx = curraccel.x;
				dy = curraccel.y;
				magf = sqrtf(dx*dx + dy*dy);

				perpdir.x *= magf;
				perpdir.y *= magf;

				//perpdir.x /= 10;
				//perpdir.y /= 10;

				perpdir.x /= 10.0f;
				perpdir.y /= 10.0f;

				nextpt.x = currpt.x + perpdir.x;
				nextpt.y = currpt.y + perpdir.y;

				//dx = nextpt.x - currpt.x;
				//dy = nextpt.y - currpt.y;
				magf = sqrtf(perpdir.x*perpdir.x + perpdir.y*perpdir.y);

				//for (linei = 0; linei <= magf * SCALE; ++linei)
				for (linei = 0; linei <= 1 * SCALE; ++linei)
				{
					x = SCALE*(currpt.x + (float)perpdir.x / (SCALE / 1) * (float)linei - MINX);
					y = SCALE*(currpt.y + (float)perpdir.y / (SCALE / 1) * (float)linei - MINY);

					rgb[(3 * (x + y*RGBX) + 0) % (3 * RGBX*RGBY)] = 255 * (1 - (float)expansioni / (float)numexpand);
					rgb[(3 * (x + y*RGBX) + 1) % (3 * RGBX*RGBY)] = 255 * (1 - (float)startptj / (float)numpts);
					rgb[(3 * (x + y*RGBX) + 2) % (3 * RGBX*RGBY)] = 255 * (1 - (float)startpti / (float)numpts);
				}

				currpt.x = nextpt.x;
				currpt.y = nextpt.y;
			}
		}
	}
	//return;
	for (massi = 0; massi < MASSES; massi++)
	{
		for (x = masses[massi].p.x*SCALE - MINX*SCALE - masses[massi].gm; x < masses[massi].p.x*SCALE - MINX*SCALE + masses[massi].gm; ++x)
		{
			for (y = masses[massi].p.y*SCALE - MINY*SCALE - masses[massi].gm; y < masses[massi].p.y*SCALE - MINY*SCALE + masses[massi].gm; ++y)
			{
				dx = x - masses[massi].p.x*SCALE - MINY*SCALE;
				dy = y - masses[massi].p.y*SCALE - MINY*SCALE;
				magf = sqrtf(dx*dx + dy*dy);
				//if (magf > masses[massi].gm)
					//continue;

				rgb[(3 * (x + y*RGBX) + 0) % (3 * RGBX*RGBY)] = 0;
				rgb[(3 * (x + y*RGBX) + 1) % (3 * RGBX*RGBY)] = 0;
				rgb[(3 * (x + y*RGBX) + 2) % (3 * RGBX*RGBY)] = 0;
			}
		}
	}

	fclose(fp);

	for (x = 0; x < RGBX; ++x)
	{
		for (y = 0; y < RGBY/2; ++y)
		{
			temprgb[0] = rgb[3 * (x + y*RGBX) + 0];
			temprgb[1] = rgb[3 * (x + y*RGBX) + 1];
			temprgb[2] = rgb[3 * (x + y*RGBX) + 2];

			rgb[3 * (x + y*RGBX) + 0] = rgb[3 * (x + (RGBY - y - 1)*RGBX) + 0];
			rgb[3 * (x + y*RGBX) + 1] = rgb[3 * (x + (RGBY - y - 1)*RGBX) + 1];
			rgb[3 * (x + y*RGBX) + 2] = rgb[3 * (x + (RGBY - y - 1)*RGBX) + 2];

			rgb[3 * (x + (RGBY - y - 1)*RGBX) + 0] = temprgb[0];
			rgb[3 * (x + (RGBY - y - 1)*RGBX) + 1] = temprgb[1];
			rgb[3 * (x + (RGBY - y - 1)*RGBX) + 2] = temprgb[2];
		}
	}

	savepng("out.png", rgb, RGBX, RGBY, 3);

	printf("press to quit");
	fgets(foo, 2, stdin);
}







