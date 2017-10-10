#include <platform/win32/system/platform_win32_null_renderer.h>
#include "fbx_loader.h"
#include <graphics/scene.h>
#include <iostream>




int main(int argc, char* argv[])
{
	gef::PlatformWin32NullRenderer platform;

	char* output_filename = "output.scn";
	char* input_filename = "";
	bool animation_only = false;


	gef::FBXLoader fbx_loader;


	for(int arg_num=0; arg_num < argc; ++arg_num)
	{
		if(arg_num == argc-1)
		{
			input_filename = argv[arg_num];
		}
		else if(argv[arg_num][0] == '-' && (strlen(argv[arg_num]) > 1))
		{
			switch(argv[arg_num][1])
			{
			case 'o':
				{
					if(arg_num < argc - 2)
					{
						output_filename = argv[arg_num+1];
					}
				}
				break;


			case 'a':
				if(stricmp(&argv[arg_num][1], "animation-only") == 0)
				{
					animation_only = true;
				}
				else if (stricmp(&argv[arg_num][1], "axis") == 0)
				{
					fbxsdk::FbxAxisSystem::EUpVector up_vector = fbxsdk::FbxAxisSystem::eYAxis;
					fbxsdk::FbxAxisSystem::EFrontVector front_vector = fbxsdk::FbxAxisSystem::eParityOdd;
					fbxsdk::FbxAxisSystem::ECoordSystem coord_system = fbxsdk::FbxAxisSystem::eRightHanded;


					if ((arg_num + 3) < (argc - 1))
					{
						// up axis
						{
							int argument_start = 0;
							int sign = 1;
							if (argv[arg_num + 1][0] == '-')
							{
								argument_start = 1;
								sign = -1;
							}

							switch (argv[arg_num + 1][argument_start])
							{
							case 'x':
								up_vector = (fbxsdk::FbxAxisSystem::EUpVector)(sign*fbxsdk::FbxAxisSystem::eXAxis);
								break;
							case 'y':
								up_vector = (fbxsdk::FbxAxisSystem::EUpVector)(sign*fbxsdk::FbxAxisSystem::eYAxis);
								break;
							case 'z':
								up_vector = (fbxsdk::FbxAxisSystem::EUpVector)(sign*fbxsdk::FbxAxisSystem::eZAxis);
								break;
							}
						}

						// parity
						{
							int argument_start = 0;
							int sign = 1;
							if (argv[arg_num + 2][0] == '-')
							{
								argument_start = 1;
								sign = -1;
							}

							if (stricmp(&argv[arg_num + 2][argument_start], "odd") == 0)
								front_vector = (fbxsdk::FbxAxisSystem::EFrontVector)(sign*fbxsdk::FbxAxisSystem::eParityOdd);
							else if (stricmp(&argv[arg_num + 2][argument_start], "even") == 0)
								front_vector = (fbxsdk::FbxAxisSystem::EFrontVector)(sign*fbxsdk::FbxAxisSystem::eParityEven);
						}
						// handed
						if (stricmp(&argv[arg_num + 3][0], "left") == 0)
							coord_system = fbxsdk::FbxAxisSystem::eLeftHanded;
						else if (stricmp(&argv[arg_num + 3][0], "right") == 0)
							coord_system = fbxsdk::FbxAxisSystem::eRightHanded;

						fbxsdk::FbxAxisSystem axis_system(up_vector, front_vector, coord_system);
						fbx_loader.SetAxisSystem(axis_system);
					}
				}
				else if (stricmp(&argv[arg_num][1], "axis-preset") == 0)
				{
					fbxsdk::FbxAxisSystem::EPreDefinedAxisSystem predefined_axis_system = fbxsdk::FbxAxisSystem::eMayaYUp;
					if ((arg_num + 1) < (argc - 1))
					{
						if (stricmp(&argv[arg_num + 1][0], "mayaZup") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eMayaZUp;
						else if (stricmp(&argv[arg_num + 1][0], "mayaYup") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eMayaYUp;
						else if (stricmp(&argv[arg_num + 1][0], "max") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eMax;
						else if (stricmp(&argv[arg_num + 1][0], "motionbuilder") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eMotionBuilder;
						else if (stricmp(&argv[arg_num + 1][0], "opengl") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eOpenGL;
						else if (stricmp(&argv[arg_num + 1][0], "directx") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eDirectX;
						else if (stricmp(&argv[arg_num + 1][0], "lightwave") == 0)
							predefined_axis_system = fbxsdk::FbxAxisSystem::eLightwave;

						fbxsdk::FbxAxisSystem axis_system(predefined_axis_system);
						fbx_loader.SetAxisSystem(axis_system);
					}
				}
				break;

			case 'e':
				if(stricmp(&argv[arg_num][1], "enable-skinning") == 0)
				{
					fbx_loader.set_ignore_skinning(false);
				}
				break;

			case 's':
				if(stricmp(&argv[arg_num][1], "strip-texture-path") == 0)
				{
					fbx_loader.set_strip_texture_path(true);
				}
				else if (stricmp(&argv[arg_num][1], "scaling-factor") == 0)
				{
					float scaling_factor = atof(argv[arg_num + 1]);
					if (scaling_factor != 0.0f)
						fbx_loader.set_scaling_factor(scaling_factor);
				}
				break;

			case 't':
				if(stricmp(&argv[arg_num][1], "texture-extension") == 0)
				{
					if(arg_num < argc - 2)
					{
						fbx_loader.set_texture_filename_ext(&argv[arg_num+1][0]);
					}
				}
				break;

			}
		}
	}

	gef::Scene scene;

	std::cout << std::endl << "FBX to Abertay Framework Scene Builder v0.01" << std::endl << std::endl;;

	std::cout << "input file: " << input_filename << std::endl;
	std::cout << "output file: " << output_filename << std::endl << std::endl;;


	std::cout << "Loading file: " << input_filename << std::endl;
	bool success = fbx_loader.Load(input_filename, platform, scene, animation_only);

	if(success)
	{
		std::cout << "file: " << input_filename << " loaded." << std::endl << std::endl;
		std::cout << "Writing output file: " << output_filename << std::endl;
		success = scene.WriteSceneToFile(platform, output_filename);
		if(success)
			std::cout << "Success." << std::endl;
		else
			std::cout << "ERROR: failed to write output file: " << output_filename << std::endl;
	}
	else
		std::cout << "ERROR: failed to load input file: " << input_filename << std::endl;


	return success == false ? -1 : 0;
}