#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "em_geojson.h"
#include "em_print.h"

em_jelem_t elem_int = {"aa", EM_JSON_INT, {.integer = 10}};
em_jelem_t elem_float = {"bb", EM_JSON_FLOAT, {.floating = 12.3}};
em_jelem_t elem_string = {"cc", EM_JSON_STRING, {.string = "xyz"}};

int jsonelem_int_test()
{
	char json_str[128];

	int ret = em_json_snprint_elem(json_str, 128, &elem_int);
	printf("ret: %s\n", json_str);

	char ans_str[128] = "\"aa\": 10";

	if (strcmp(json_str, ans_str) != 0)
	{
		printf("ans: %s\n", ans_str);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	return 0;
}

int jsonelem_float_test()
{
	char json_str[128];

	em_json_snprint_elem(json_str, 128, &elem_float);
	printf("ret: %s\n", json_str);

	char ans_str[128] = "\"bb\": 12.300000";

	if (strcmp(json_str, ans_str) != 0)
	{
		printf("ans: %s\n", ans_str);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	return 0;
}

int jsonelem_string_test()
{
	char json_str[128];

	em_json_snprint_elem(json_str, 128, &elem_string);
	printf("ret: %s\n", json_str);

	char ans_str[128] = "\"cc\": \"xyz\"";

	if (strcmp(json_str, ans_str) != 0)
	{
		printf("ans: %s\n", ans_str);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	return 0;
}

int jsonobj_obj_test()
{
	em_jelem_t *elems[3] = {&elem_int, &elem_float, &elem_string};
	em_jobj_t obj = {3, elems};

	char json_str[128];

	em_json_snprint_obj(json_str, 128, &obj);
	printf("ret: %s\n", json_str);

	char ans_str[128] = "{ \"aa\": 10, \"bb\": 12.300000, \"cc\": \"xyz\" }";

	if (strcmp(json_str, ans_str) != 0)
	{
		printf("ans: %s\n", ans_str);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	return 0;
}

int geometry_point_test()
{
	double lon = 130.123456789;
	double lat = 35.123456789;
	em_gjpoint_t geopoint = {lon, lat};

	em_gjgeometry_t geometry = {EM_GJ_POINT, {&geopoint}};

	char json_str[128];
	em_geojson_snprint_geometry(json_str, 128, &geometry);
	printf("ret: %s\n", json_str);

	char ans_str[128] = "\"geometry\": { \"type\": \"Point\", \"coordinates\": [130.123457, 35.123457] }";

	if (strcmp(json_str, ans_str) != 0)
	{
		printf("ans: %s\n", ans_str);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	return 0;
}

int feature_point_test()
{
	double lon = 130.123456789;
	double lat = 35.123456789;
	em_gjpoint_t geopoint = {lon, lat};
	em_gjgeometry_t geometry = {EM_GJ_POINT, {&geopoint}};

	em_gjfeature_t feature = {&geometry, NULL};

	char json_str[256];
	em_geojson_snprint_feature(json_str, 256, &feature);
	printf("ret: %s\n", json_str);

	char ans_str[256] = "{ \"type\": \"Feature\", \"geometry\": { \"type\": \"Point\", \"coordinates\": [130.123457, 35.123457] } }";

	if (strcmp(json_str, ans_str) != 0)
	{
		printf("ans: %s\n", ans_str);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	// properties有

	em_jelem_t *elems[3] = {&elem_int, &elem_float, &elem_string};
	em_jobj_t obj = {3, elems};
	feature.properties = &obj;

	em_geojson_snprint_feature(json_str, 256, &feature);
	printf("ret: %s\n", json_str);

	char ans_str2[256] = "{ \"type\": \"Feature\", \"geometry\": { \"type\": \"Point\", \"coordinates\": [130.123457, 35.123457] }, \"properties\": { \"aa\": 10, \"bb\": 12.300000, \"cc\": \"xyz\" } }";

	if (strcmp(json_str, ans_str2) != 0)
	{
		printf("ans: %s\n", ans_str2);
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_printf(EM_LOG_TOP, "OK\n");

	return 0;
}

int parse_geopoint_test()
{
	double lon;
	double lat;
	char json_str[128] = "\"geometry\": { \"type\": \"Point\", \"coordinates\": [130.123457, 35.123457] }";

	int ret = em_parse_geojson_point(&lon, &lat, json_str);

	em_printf(EM_LOG_TOP, "%lf, %lf (%d)\n", lon, lat, ret);

	return 0;
}

int main()
{
	em_printf(EM_LOG_TOP, "geojson test start\n");

	jsonelem_int_test();
	jsonelem_float_test();
	jsonelem_string_test();
	jsonobj_obj_test();

	geometry_point_test();

	feature_point_test();

	parse_geopoint_test();

	em_printf(EM_LOG_TOP, "geojson test end\n");

	return 0;
}