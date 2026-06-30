#include "Utils.h"

#include <vector>
#include <algorithm>
#include <filesystem>


bool IntersectAABB(const AABB& a, const AABB& b, AABB& c)
{
	if(a.min.x < b.max.x && a.max.x > b.min.x && a.min.y < b.max.y && a.max.y > b.min.y)
	{
		c.min.x = fmaxf(a.min.x, b.min.x);
		c.min.y = fmaxf(a.min.y, b.min.y);
		c.max.x = fminf(a.max.x, b.max.x);
		c.max.y = fminf(a.max.y, b.max.y);
		return true;
	}
	else
	{
		c = {glm::vec2(0,0),glm::vec2(0,0)};
	}

	return false;
}

/**
 * calculates the necessary transforms that transform from -> to
 */
void CalculateRelativeTransformAABB(const AABB& from, const AABB& to, glm::vec2& scale, glm::vec2& offset)
{
	float fromW = (from.max.x - from.min.x);
	float fromH = (from.max.y - from.min.y);
	float toW = (to.max.x - to.min.x);
	float toH = (to.max.y - to.min.y);

	scale.x =  toW / fromW;
	scale.y = toH / fromH;

	offset.x = (to.min.x - from.min.x) / fromW;
	offset.y = (to.min.y - from.min.y) / fromH;
}

/**
 * Transform an AABB based on scaling and position offset.
 */
void TransformAABB(AABB& a, glm::vec2& scale, glm::vec2& offset)
{
	float aW = (a.max.x - a.min.x);
	float aH = (a.max.y - a.min.y);

	float aScaledW = aW * scale.x;
	float aScaledH = aH * scale.y;

	float newMinX = a.min.x + offset.x * aW;
	float newMinY = a.min.y + offset.y * aH;

	a.min.x = newMinX;
	a.min.y  = newMinY;
	a.max.x = newMinX + aScaledW;
	a.max.y = newMinY + aScaledH;
}

/**
 * This function is an amalgamation of:
 * - calculating the needed transform that turns start -> end
 * - using this transform on
 */
void TransformBasedOnTwoRelativeAABB(const AABB& start, const AABB& end, AABB& toTransform)
{
	glm::vec2 scale;
	glm::vec2 offset;
	CalculateRelativeTransformAABB(start,end,scale,offset);
	TransformAABB(toTransform,scale,offset);
}

bool IsPointInsideAABB(const AABB &box, const glm::vec2 &point)
{
	return point.x >= box.min.x && point.x <= box.max.x && point.y >= box.min.y && point.y <= box.max.y;
}

AABB GetRectangleOfCharacter(char c)
{
	AABB retval = {glm::vec2(0.0f,0.0f),glm::vec2(0.1f,0.1f)};

    if(c >= ' ' && c<= '~')
    {    
        if(c != 'M' && c!= 'W' && c!= 'w' && c!= 'm')
        {
            int charDiff = c - ' ' - (char)1;

            float x = 0.1f * (charDiff % 10);
            float y = 0.1f * (charDiff / 10);

            retval.min = glm::vec2(x,y);
            retval.max = retval.min + glm::vec2(0.1f,0.1f);
        }
        else
        {
            if(c== 'M')
            {
                retval.min=glm::vec2(0.4f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'W')
            {
                retval.min=glm::vec2(0.55f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'm')
            {
                retval.min=glm::vec2(0.70f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
            else if(c == 'w')
            {
                retval.min=glm::vec2(0.85f,0.9f);
                retval.max = retval.min + glm::vec2(0.15f,0.1f);
            }
        }
    }

    return retval;
}

//This functions takes in vec2 raw screen coordinates,
//And spits out the correct NDC coordinates
static glm::vec2 ScreenToNDC(const glm::vec2& rawScreenCoords, int windowWidth, int windowHeight)
{
	float normalizedDeviceX = (rawScreenCoords.x / (float)windowWidth) * 2 - 1;
	float normalizedDeviceY = ((rawScreenCoords.y / (float)windowHeight) * 2 - 1) * -1;

	return glm::vec2(normalizedDeviceX, normalizedDeviceY);
}

static glm::vec2 NDCToScreen(const glm::vec2& normalizedDeviceCoords, int windowWidth, int windowHeight)
{
	float screenX = (normalizedDeviceCoords.x + 1.0f) / 2.0f * windowWidth;
	float screenY = (-normalizedDeviceCoords.y + 1.0f) / 2.0f * windowHeight;

	return glm::vec2(screenX, screenY);
}

//Has numerical stability(division by zero avoidance)
//Calculates u interpolated: p1*(1-u) + p2 * u
static float HorizontalRayToLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& p1, const glm::vec2& p2)
{
	//we know our ray's direction is: vec2(1,0)

	//If this happens we know that the line segment is almost paralell to our ray
	//Meaning no collision, therefore we return -1
	if (fabsf(p1.y - p2.y) < 0.0000001f)
	{
		return -1;
	}

	float u = (rayOrigin.y - p1.y) / (p2.y - p1.y);
	if (u >= 0 && u <= 1)
	{
		float t = p1.x - rayOrigin.x + u * (p2.x - p1.x);
		return t;
	}
	
	return -1;
	
}

//Returns 0<=t<=1 if ray intersects the line segment between p1 and p2
//Yet to be written
static float RayLineIntersect(const glm::vec2& rayOrigin, const glm::vec2& rayDirection, const glm::vec2& p1, const glm::vec2& p2)
{
	return -1.0f;
}

//Turns a positive (0<= integer) into string
static std::string IntegerToString(int integer)
{
	if (integer < 0)
	{
		return "";
	}
	if (integer == 0)
	{
		return "0";
	}
	char zeroAscii = '0';
	std::string retval = "";
	int number = integer;
	while (number > 0)
	{
		int currentNumber = number % 10;
		number /= 10;

		retval = (char)(zeroAscii + currentNumber) + retval;
	}

	


	return retval;
}

//Return -1 if the string is invalid
static int StringToInteger(const std::string& string)
{
	if (string.size() < 1)
	{
		return -1;
	}
	if (string.size() == 1 && string[0] == '0')
	{
		return 0;
	}

	if (string[0] == '0')
	{
		//cant start with 0
		return -1;
	}
	
	char zeroAscii = '0';
	int retval = 0;
	for (int i = 0;i < string.size();++i)
	{
		if (string[i] > '9' || string[i] < '0')
		{
			return -1;
		}
		retval = retval * 10 + (int)(string[i] - zeroAscii);
	}
		
	return retval;

}

namespace fs = std::filesystem;

void GetFileNamesWithSpecificExtension(const std::string &folderPath, const std::vector<std::string> &extensions, std::vector<std::string> &outputFileNames)
{
	outputFileNames.clear();

	for(const auto& directoryEntry : fs::directory_iterator(folderPath))
	{
		if(directoryEntry.is_regular_file())
		{
			std::string extensionOfFile = directoryEntry.path().extension().string();

			if(std::find(extensions.begin(),extensions.end(),extensionOfFile) != extensions.end())
			{
				//we found a good file, so we save it
				outputFileNames.push_back(directoryEntry.path().filename().string());
			}
		}
	}
}
