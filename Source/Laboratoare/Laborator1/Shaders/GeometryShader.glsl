#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 170) out;

layout(location = 0) in vec2 v_texture_coord[];

uniform mat4 View;
uniform mat4 Projection;

uniform int instances;
uniform float shrink_unit;

layout(location = 0) out vec2 texture_coord;

void EmitPoint(vec3 pos, vec3 offset)
{
	gl_Position = Projection * View * vec4(pos + offset, 1.0);
	EmitVertex();
}

void main()
{
	vec3 p1 = gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[1].gl_Position.xyz;
	vec3 p3 = gl_in[2].gl_Position.xyz;

	vec3 center = (p1+p2+p3)/vec3(3,3,3);

	for (int i = 0; i <= instances; i++)
	{
		//TODO modify offset so that instances are displayed on 6 columns

		//matricea de instante are 6 bambusi per linie => 6 coloane, se construieste spre z negativ
		//apasand tasta = se adauga bambusi, apasand tasta - se scad
		vec3 offset = vec3(i % 6, 0, -i/6);

		//TODO modify the points so that the triangle shrinks relative to its center

		texture_coord = v_texture_coord[0];
		p1 = p1 + (center - p1) * vec3(shrink_unit,shrink_unit,shrink_unit);
		EmitPoint(p1, offset);

		texture_coord = v_texture_coord[1];
		p2 = p2 + (center - p2) * vec3(shrink_unit,shrink_unit,shrink_unit);
		EmitPoint(p2, offset);

		texture_coord = v_texture_coord[2];
		p3 = p3 + (center - p3) * vec3(shrink_unit,shrink_unit,shrink_unit);
		EmitPoint(p3, offset);

		EndPrimitive();
	}
}
