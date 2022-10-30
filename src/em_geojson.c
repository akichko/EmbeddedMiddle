/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "em_geojson.h"
#include "em_print.h"

int em_json_snprint_elem(char *dst, int len, em_jelem_t *elem)
{
	int offset = 0;
	int size;

	size = snprintf(dst + offset, len - offset, "\"%s\": ", elem->key);
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	switch (elem->type)
	{
	case EM_JSON_INT:
		size = snprintf(dst + offset, len - offset, "%d", elem->val.integer);
		break;
	case EM_JSON_FLOAT:
		size = snprintf(dst + offset, len - offset, "%lf", elem->val.floating);
		break;
	case EM_JSON_STRING:
		size = snprintf(dst + offset, len - offset, "\"%s\"", elem->val.string);
		break;
	case EM_JSON_OBJ:
		// size = snprintf(dst + offset, len - offset, "\"%s\": \"%s\"", elem->key, elem->val.string);
		// break;
	default:
		em_printf(EM_LOG_ERROR, "unknown type\n");
		break;
	}
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;
	return offset;
}

int em_json_snprint_obj(char *dst, int len, em_jobj_t *obj)
{
	int offset = 0;
	int size;

	size = snprintf(dst + offset, len - offset, "{ ");
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	for (int i = 0; i < obj->num_elems; i++)
	{
		size = em_json_snprint_elem(dst + offset, len - offset, obj->elems[i]);
		if (size <= 0 || size >= len - offset)
		{
			return -1;
		}
		offset += size;

		size = snprintf(dst + offset, len - offset, ", ");
		if (size <= 0 || size >= len - offset)
		{
			return -1;
		}
		offset += size;
	}

	size = snprintf(dst + offset - 2, 3, " }");

	return offset;
}

int em_geojson_snprint_point(char *dst, int len, em_gjpoint_t *gp)
{
	int offset = 0;

	int size = snprintf(dst + offset, len - offset, "{ \"type\": \"Point\", \"coordinates\": [%lf, %lf] }",
						gp->lon, gp->lat);
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;
	return offset;
}

int em_geojson_snprint_geometry(char *dst, int len, em_gjgeometry_t *geometry)
{
	int offset = 0;
	int size;

	size = snprintf(dst + offset, len - offset, "\"geometry\": ");
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	switch (geometry->type)
	{
	case EM_GJ_POINT:
		size = em_geojson_snprint_point(dst + offset, len - offset, geometry->val.point);
		break;

	default:
		em_printf(EM_LOG_ERROR, "unknown geometry type");
		return -1;
	}
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	return offset;
}

int em_geojson_snprint_feature(char *dst, int len, em_gjfeature_t *gf)
{
	int offset = 0;
	int size;

	size = snprintf(dst + offset, len - offset, "{ \"type\": \"Feature\", ");
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	size = em_geojson_snprint_geometry(dst + offset, len - offset, gf->geometry);
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	if (gf->properties != NULL)
	{
		size = snprintf(dst + offset, len - offset, ", \"properties\": ");
		if (size <= 0 || size >= len - offset)
		{
			return -1;
		}
		offset += size;

		size = em_json_snprint_obj(dst + offset, len - offset, gf->properties);
		if (size <= 0 || size >= len - offset)
		{
			return -1;
		}
		offset += size;
	}

	size = snprintf(dst + offset, len - offset, " }");
	if (size <= 0 || size >= len - offset)
	{
		return -1;
	}
	offset += size;

	return offset;
}



int em_parse_geojson_point(double *lon, double *lat, char *src)
{
	char *start = strstr(src, "[");
	int ret = sscanf(start, "[ %lf , %lf ]", lon, lat);

	return ret;
}
