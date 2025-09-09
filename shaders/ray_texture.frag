#version 450 core

layout(binding=1) uniform sampler2D tex;
uniform uvec2 windowSize;

out vec4 FragColor;

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

void main()
{	
	//vec4 tl = texture(tex, (gl_FragCoord.xy + vec2(-1.0, 1.0))/windowSize);
    //vec4 tc = texture(tex, (gl_FragCoord.xy + vec2( 0.0, 1.0))/windowSize);
    //vec4 tr = texture(tex, (gl_FragCoord.xy + vec2( 1.0, 1.0))/windowSize);
    //vec4 ml = texture(tex, (gl_FragCoord.xy + vec2(-1.0, 0.0))/windowSize);
    //vec4 mc = texture(tex, (gl_FragCoord.xy + vec2( 0.0, 0.0))/windowSize);
    //vec4 mr = texture(tex, (gl_FragCoord.xy + vec2( 1.0, 0.0))/windowSize);
    //vec4 bl = texture(tex, (gl_FragCoord.xy + vec2(-1.0,-1.0))/windowSize);
    //vec4 bc = texture(tex, (gl_FragCoord.xy + vec2( 0.0,-1.0))/windowSize);
    //vec4 br = texture(tex, (gl_FragCoord.xy + vec2( 1.0,-1.0))/windowSize);
    //
    //FragColor = tl * kernel[0] + tc * kernel[1] + tr * kernel[2] +
    //            ml * kernel[3] + mc * kernel[4] + mr * kernel[5] +
    //            bl * kernel[6] + bc * kernel[7] + br * kernel[8];

    FragColor = texture(tex, gl_FragCoord.xy/windowSize);
}