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
#ifndef __EM_GEOJSON_H__
#define __EM_GEOJSON_H__

#include "em_cmndefs.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

	typedef enum
	{
		EM_JSON_INT,
		EM_JSON_FLOAT,
		EM_JSON_STRING,
		EM_JSON_OBJ
	} eme_json_type_t;

	typedef struct _em_jobj_t em_jobj_t;

	typedef union
	{
		int integer;
		double floating;
		char *string;
		em_jobj_t *jobj;
	} emu_jsonval_t;

	typedef struct
	{
		char *key;
		eme_json_type_t type;
		emu_jsonval_t val;
	} em_jelem_t;

	typedef struct _em_jobj_t
	{
		int num_elems;
		em_jelem_t **elems;
	} em_jobj_t;

	typedef struct
	{
		char *key;
		eme_json_type_t type;
		double value;
	} em_jdouble_t;

	// geojson

	typedef enum
	{
		EM_GJ_POINT,
		EM_GJ_MULTIPOINT,
		EM_GJ_FEATURE
	} eme_geojson_type_t;

	typedef struct
	{
		double lon;
		double lat;
	} em_gjpoint_t;

	typedef struct
	{
		eme_geojson_type_t type;
		union em_u_geotype_t
		{
			em_gjpoint_t *point;
		} val;
	} em_gjgeometry_t;

	typedef struct
	{
		// eme_geojson_type_t type;
		em_gjgeometry_t *geometry;
		em_jobj_t *properties;
	} em_gjfeature_t;

	typedef struct
	{
		// eme_geojson_type_t type;
		int num_features;
		em_gjfeature_t **features;
	} em_gjfeaturecollection_t;

	int em_json_snprint_elem(char *dst,
							 int len,
							 em_jelem_t *elem);

	int em_json_snprint_obj(char *dst,
							int len,
							em_jobj_t *obj);

	int em_geojson_snprint_point(char *dst,
								 int len,
								 em_gjpoint_t *gp);

	int em_geojson_snprint_geometry(char *dst,
									int len,
									em_gjgeometry_t *geometry);

	int em_geojson_snprint_feature(char *dst,
								   int len,
								   em_gjfeature_t *gf);

	int em_parse_geojson_point(double *lon, double *lat, char *src);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //__EM_GEOJSON_H__
