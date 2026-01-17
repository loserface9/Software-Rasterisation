#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"


struct Instruction_Counts {
    int num_v, num_vt, num_vn, num_f;
};
struct fline_Output {
    char *contents;
    bool end_of_file;
};

static struct fline_Output get_fline(FILE *stream);
static struct Instruction_Counts get_instruction_counts (const char *filepath);
static void get_instruction_from_line (char *restrict out, const char *restrict line, const int line_len);
static int string_to_Vec3 (Vec3 *restrict output, const char *restrict string);
static int string_to_Face (Face *output, const char *restrict string, const Obj *object);

const Face Face_NULL = {NULL, NULL, NULL, -1};
const Obj Obj_NULL = {NULL, NULL, NULL, NULL, -1, -1, -1, -1};


Obj import_obj (const char *filepath) {
    struct Requires_Deallocation {
        bool object;
        bool fptr;
    };
    struct Requires_Deallocation requires_deallocation = {false, false};

    const struct Instruction_Counts instruction_counts = get_instruction_counts(filepath);
    if (instruction_counts.num_v < 0) goto error;

    FILE *fptr = fopen(filepath, "r");
    if (fptr == NULL) {
        printf("file obj.c, function import_obj():\n\tFailed to open file.\n");
        goto error;
    }
    requires_deallocation.fptr = true;

    Obj object = {
        malloc(instruction_counts.num_v * sizeof(Vec3)),
        malloc(instruction_counts.num_vt * sizeof(Vec3)),
        malloc(instruction_counts.num_vn * sizeof(Vec3)),
        malloc(instruction_counts.num_f * sizeof(Face)),
        0, 0, 0, 0
    };
    requires_deallocation.object = true;

    struct fline_Output curr_fline_output = get_fline(fptr);
    while (!curr_fline_output.end_of_file) {
        const char *curr_line = curr_fline_output.contents;
        const int curr_line_len = strlen(curr_line);

        // instruction is normally a string like "v" or "vn" or "f"
        char instruction[curr_line_len];
        get_instruction_from_line(instruction, curr_line, curr_line_len);
        const int instruction_str_len = strlen(instruction);

        // curr_line now points to the start of the numeric data
        curr_line += instruction_str_len + 1;

        if ((instruction[0] == 'v') & (instruction[1] == '\0')) {
            // Vertex
            Vec3 *vertex_ptr = &object.vertices[object.num_v];
            string_to_Vec3(vertex_ptr, curr_line);
            object.num_v++;
        } else if ((instruction[0] == 'v') & (instruction[1] == 't') & (instruction[2] == '\0')) {
            // Vertex texture
            Vec3 *vertex_texture_ptr = &object.vertex_textures[object.num_vt];
            string_to_Vec3(vertex_texture_ptr, curr_line);
            object.num_vt++;
        } else if ((instruction[0] == 'v') & (instruction[1] == 'n') & (instruction[2] == '\0')) {
            // Vertex normal
            Vec3 *vertex_normal_ptr = &object.vertex_normals[object.num_vn];
            string_to_Vec3(vertex_normal_ptr, curr_line);
            object.num_vn++;
        } else if ((instruction[0] == 'f') & (instruction[1] == '\0')) {
            // Face
            Face *face_ptr = &object.faces[object.num_f];
            string_to_Face(face_ptr, curr_line, &object);
            object.num_f++;
        }

        free(curr_fline_output.contents);
        curr_fline_output = get_fline(fptr);
    }
    free(curr_fline_output.contents);

    if (requires_deallocation.fptr) fclose(fptr);
    return object;

    error:
        if (requires_deallocation.fptr) fclose(fptr);
        if (requires_deallocation.object) Obj_free(&object);
        object = Obj_NULL;
        return object;
}


static struct fline_Output get_fline(FILE *stream) {
/* Take a file pointer as input
 * Output contents of file until next newline or end of file.
 * Does not include newline character in the output.
 * File pointer will be incremented to the beginning of the next line
 */

    const int initial_buffer_length = 100;

    char *contents = malloc(initial_buffer_length * sizeof(char));
    int buffer_length = initial_buffer_length;
    int curr_char = fgetc(stream);
    int char_idx = 0;
    while ((curr_char != '\n') & (curr_char != EOF)) {
        // Check whether to increase the size of the buffer
        if (char_idx == buffer_length) {
            buffer_length += initial_buffer_length;
            contents = realloc(contents, buffer_length * sizeof(char));
        }

        // Assign the char to the string
        contents[char_idx] = (char) curr_char;

        // Fetch next char
        curr_char = fgetc(stream);
        char_idx++;
    }

    const int contents_length = char_idx;
    contents = realloc(contents, (contents_length + 1) * sizeof(char));
    contents[contents_length] = '\0';

    const struct fline_Output output = {
        contents, curr_char == EOF
    };
    return output;
}


void Obj_free (Obj *object) {
    for (int face_idx = 0; face_idx < object->num_f; face_idx++) {
        Face_free(&object->faces[face_idx]);
    }

    free(object->vertices);
    free(object->vertex_textures);
    free(object->vertex_normals);
    free(object->faces);
}


void Face_free (Face *face) {
    free(face->vertices);
    free(face->vertex_textures);
    free(face->vertex_normals);
}


static struct Instruction_Counts get_instruction_counts (const char *filepath) {
    struct Instruction_Counts output = {0, 0, 0, 0};

    FILE *fptr = fopen(filepath, "r");
    if (fptr == NULL) {
        printf("file obj.c, function get_instruction_counts():\n\tFailed to open file.\n");
        output.num_v = -1; output.num_vt = -1; output.num_vn = -1; output.num_f = -1;
        return output;
    }

    struct fline_Output fline_output = get_fline(fptr);
    while (!fline_output.end_of_file) {
        const char *line = fline_output.contents;
        const int line_len = strlen(line);

        // instruction is normally a string like "v" or "vn" or "f"
        char instruction[line_len];
        get_instruction_from_line(instruction, line, line_len);

        if ((instruction[0] == 'v') & (instruction[1] == '\0')) {
            output.num_v++;
        } else if ((instruction[0] == 'v') & (instruction[1] == 't') & (instruction[2] == '\0')) {
            output.num_vt++;
        } else if ((instruction[0] == 'v') & (instruction[1] == 'n') & (instruction[2] == '\0')) {
            output.num_vn++;
        } else if ((instruction[0] == 'f') & (instruction[1] == '\0')) {
            output.num_f++;
        }

        free(fline_output.contents);
        fline_output = get_fline(fptr);
    }
    free(fline_output.contents);

    return output;
}


static void get_instruction_from_line (char *restrict out, const char *restrict line, const int line_len) {
// Get a .obj file instruction from a string

    // instruction is normally a string like "v" or "vn" or "f"
    int i = 0;
    while ((line[i] != ' ') & (line[i] != '\0')) {
        out[i] = line[i];
        i++;
    }

    for (int j = i; j < line_len; j++) {
        out[j] = '\0';
    }
}


static int string_to_Vec3 (Vec3 *restrict output, const char *restrict string) {
// string should initially point to the first float, or whitespace before the first float

    const char *curr_float = string;

    for (int dim = 0; dim < 3; dim++) {
        while (curr_float[0] == ' ') curr_float++;
        bool is_negative = curr_float[0] == '-';
        curr_float += is_negative;

        // Position of DP and space
        const int dp_pos = strcspn(curr_float, "."), space_pos = strcspn(curr_float, " ");
        // Length of each part of the float
        const int before_dp_len = dp_pos,
                  after_dp_len = space_pos - dp_pos - 1;
        char before_dp[before_dp_len+1], after_dp[after_dp_len+1];

        // Fetch digits before DP
        for (int before_dp_idx = 0; before_dp_idx < before_dp_len; before_dp_idx++) {
            before_dp[before_dp_idx] = curr_float[before_dp_idx];
        }
        // Fetch digits after DP
        for (int after_dp_idx = 0; after_dp_idx < after_dp_len; after_dp_idx++) {
            after_dp[after_dp_idx] = curr_float[dp_pos + 1 + after_dp_idx];
        }


        (*output)[dim] = 0;
        // Add digits before DP
        for (int before_dp_idx = 0; before_dp_idx < before_dp_len; before_dp_idx++) {
            double power = (before_dp_len - 1) - before_dp_idx;
            double mul = pow(10, power);
            (*output)[dim] += (before_dp[before_dp_idx] - '0') * mul;
        }
        // Add digits after DP
        for (int after_dp_idx = 0; after_dp_idx < after_dp_len; after_dp_idx++) {
            double power = -after_dp_idx - 1;
            double mul = pow(10, power);
            (*output)[dim] += (after_dp[after_dp_idx] - '0') * mul;
        }

        if (is_negative) {
            (*output)[dim] = -(*output)[dim];
        }

        curr_float += space_pos + 1;
    }

    return 0;
}


static int string_to_Face (Face *output, const char *restrict string, const Obj *object) {
// string should initially point to the first number, or whitespace before the first number

    int slash_count = 0;
    for (int i = 0; string[i] != '\0'; i++) {
        slash_count += string[i] == '/';
    }
    const int num_vertices = slash_count / 2;
    output->vertices        = malloc(num_vertices * sizeof(Vec3 *));
    output->vertex_textures = malloc(num_vertices * sizeof(Vec3 *));
    output->vertex_normals  = malloc(num_vertices * sizeof(Vec3 *));
    output->num_vertices    = num_vertices;

    const char *curr_vertex_str = string;
    for (int vertex_idx = 0; vertex_idx < num_vertices; vertex_idx++) {
        while (curr_vertex_str[0] == ' ') curr_vertex_str++;

        const int slash_1_pos = strcspn(curr_vertex_str,                   "/");
        const int slash_2_pos = strcspn(curr_vertex_str + slash_1_pos + 1, "/") + slash_1_pos + 1;
        const int space_pos   = strcspn(curr_vertex_str,                   " ");
        const int v_len  = slash_1_pos,
                  vt_len = slash_2_pos - slash_1_pos - 1,
                  vn_len = space_pos - slash_2_pos - 1;


        int v_num = 0, vt_num = 0, vn_num = 0;
        // Add digits for vertices
        for (int v_idx = 0; v_idx < v_len; v_idx++) {
            const char digit = curr_vertex_str[v_idx];
            const double power = (v_len - 1) - v_idx;
            const double mul = pow(10, power);
            v_num += (digit - '0') * mul;
        }

        // Add digits for vertex textures
        for (int vt_idx = 0; vt_idx < vt_len; vt_idx++) {
            const char digit = curr_vertex_str[slash_1_pos + 1 + vt_idx];
            const double power = (vt_len - 1) - vt_idx;
            const double mul = pow(10, power);
            vt_num += (digit - '0') * mul;
        }
        // Add digits for vertex normals
        for (int vn_idx = 0; vn_idx < vn_len; vn_idx++) {
            const char digit = curr_vertex_str[slash_2_pos + 1 + vn_idx];
            const double power = (vn_len - 1) - vn_idx;
            const double mul = pow(10, power);
            vn_num += (digit - '0') * mul;
        }

        // Remove 1 from each to give zero-indexing
        v_num -= 1; vt_num -= 1; vn_num -= 1;

        // Assigned the vertex pointer
                         output->vertices[vertex_idx]        = &(object->vertices[v_num]);
        if (vt_len == 0) output->vertex_textures[vertex_idx] = NULL;
        else             output->vertex_textures[vertex_idx] = &(object->vertex_textures[vt_num]);
        if (vn_len == 0) output->vertex_normals[vertex_idx]  = NULL;
        else             output->vertex_normals[vertex_idx]  = &(object->vertex_normals[vn_num]);

        curr_vertex_str += space_pos + 1;
    }

    return 0;
}
