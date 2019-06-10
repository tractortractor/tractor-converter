#include "get_options.hpp"



namespace tractor_converter{



boost::program_options::variables_map get_options(int ac, char** av)
{
  boost::program_options::variables_map vm;

  try
  {
//  int opt;
    std::string config_file;

    // Declare a group of options that will be
    // allowed only on command line.
    boost::program_options::options_description generic("Generic options");
    generic.add_options()
      ("version,v", "\tPrint version.\n")
      ("help,h", "\tProduce help message.\n")
      ("config,c",
       boost::program_options::value<std::string>(&config_file)->
         default_value("tractor_converter.cfg"),
       "\tPath to the configuration file.\n")
      ;

    // Declare a group of options that will be
    // allowed both on command line and in
    // config file.
    boost::program_options::options_description config("Configuration");
    config.add_options()
      ("mode,m",
       boost::program_options::value<std::string>(),
       (
        "\tMode of operation."
        "\nPossibe values are:"
          "\n"
          "\n"
          "\n"
          "\n"
          "\n"
          "\n\tusage_pal - Create black-and-white color palette to indicate "
              "which colors are used for Vangers *.bmp files in source_dir."
            "\n"
            "\nBy default creates one usage palette for all files."
            "\n\"output_file\" must be specified in this case."
            "\n"
            "\nIf \"usage_pal_for_each_file\" is specified "
                "creates usage palette for each file."
            "\n\"output_dir\" must be specified in this case."
            "\n"
            "\n\"source_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tremove_not_used_pal - Use output of \"usage\" mode "
              "to create 2 palettes."
            "\n"
            "\nOne with used colors goes to \"output_dir\"."
            "\n"
            "\nOne with unused colors goes to \"output_dir_unused\"."
            "\nThose unused colors are still needed to dispaly text "
                "and other game elements properly."
            "\nSo after performing all changes you should merge unused "
                "palette back to *.tga image with tga_merge_unused_pal mode."
            "\n"
            "\n\"source_dir\", \"output_dir\", \"output_dir_unused\" and "
                "\"usage_pals_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\ttga_merge_unused_pal - merge *.tga file palette "
              "with unused colors palette."
            "\n\"source_dir\", \"output_dir\" and \"unused_pals_dir\" "
                "options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\ttga_replace_pal - replace *.tga file palette "
              "with color palette from \"pal_dir\"."
            "\n\"source_dir\", \"output_dir\" and \"pal_dir\" "
                "options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\textract_tga_pal - Get raw palettes from *.tga files."
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tvangers_pal_to_tga_pal - Convert Vangers palettes "
              "to normal *.tga ones."
            "\nPalette files in \"source_dir\" must have *.pal extension."
            "\nSpecify \"reversed\" option to convert *.tga palettes "
                "to Vangers ones."
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tpal_shift_for_vangers_avi - Move colors from 0-127 to 128-255, "
              "overwrite 0-127 with zero colors."
            "\nPalette files in \"source_dir\" must have *.pal extension."
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tcompare_bmp_escave_outside - Compare values of bytes "
              "in *.bmp images for escave and non-escave files."
            "\nCreate map to indicate which bytes of source files "
                "match bytes of compared files."
            "\nSpecify \"readable_output\" option to get human readable output."
            "\n\"source_dir\", \"dir_to_compare\" and \"output_file\" "
                "options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tbmp_to_tga - Convert Vangers *.bmp files "
              "from source_dir folder to tga files."
            "\n"
            "\nAll input *.pal files must be converted from vangers format "
                "to normal one with \"vangers_pal_to_tga_pal\" mode."
            "\n"
            "\nBy default \"pal\" option must be specifed to get "
                "single palette to use for all bmp files."
            "\n"
            "\nIf \"pal_for_each_file\" option is specified \"pal_dir\" "
                "must be specified instead of \"pal\"."
            "\nIn this case for each *.bmp file in \"source_dir\" "
                "there must be *.pal file with same name in \"pal_dir\"."
            "\n"
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\ttga_to_bmp - Convert *.tga images to Vangers *.bmp ones."
            "\nRLE compression is not supported."
            "\n"
            "\nTo convert item files add \"items_bmp\" option."
            "\nThere are 2 different images for each item: "
                "one used inside escave and one used outside."
            "\nProgram uses output of \"compare_bmp_escave_outside\" mode "
                "to create those 2 images from each *.tga file."
            "\n"
            "\nSpecify \"fix_null_bytes_and_direction\" option "
                "in case transparency null bytes were changed "
                 "to never used color or image got rotated/flipped."
            "\n"
            "\n\"source_dir\" and \"output_dir\" options must be specified."
            "\n\"map\" and \"output_dir_through_map\" options "
                "must be specified while converting items."
          "\n"
          "\n"
          "\n"
          "\n"
          "\n"
          "\n"
          "\n\tvangers_3d_model_to_obj - Convert Vangers *.m3d and *.a3d "
              "models into Wavefront *.obj ones."
            "\nFor each game directory found in \"source_dir\" "
                "output directory in \"output_dir\" is created."
            "\n"
            "\nEach *.m3d and *.a3d file contains multiple 3d object."
            "\nFor each *.m3d or *.a3d file program creates folder with all "
                "enclosed models in Wavefront object format."
            "\n"
            "\nSpecify \"3d_obj_float_precision\" option to change "
                "precision of float numbers of output *.obj files."
            "\nSpecify \"3d_default_scale\" option to set scale for objects "
                "for which there is no scale info "
                "in *.prm and game.lst files."
            "\nUse \"m3d_weapon_file\" option to specify name of *.m3d "
                "weapon model to insert into mechos models "
                "to indicate positions of weapons."
            "\nUse \"weapon_attachment_point_file\" option to specify path "
                "to wavefront *.obj file which contains model "
                "to be inserted into weapon model to "
                "indicate weapon attachment point."
            "\nUse \"ghost_wheel_file\" option to specify path "
                "to wavefront *.obj file which contains model "
                "to be inserted in place of wheels with no polygons."
            "\nSpecify \"extract_center_of_mass\" option to mark "
                "center of mass found in *.m3d/*.a3d file."
            "\nUse \"center_of_mass_file\" to specify path "
                "to wavefront *.obj file which contains model "
                "which will mark center of mass."
            "\nSpecify \"wavefront_mtl\" option to copy *.mtl file "
                "to output folders so generated *.obj files "
                "will refer to it."
            "\nSpecify \"extract_nonexistent_weapons\" option to extract "
                "weapons which are marked as nonexistent."
            "\nSpecify \"use_custom_volume_by_default\" to enable by default "
                "custom volume option in generated per-file configs."
            "\nSpecify \"use_custom_rcm_by_default\" to enable by default "
                "custom center of mass option in generated per-file configs."
            "\nSpecify \"use_custom_J_by_default\" to enable by default "
                "custom inertia tensor option in generated per-file configs."
            "\n"
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tobj_to_vangers_3d_model - Convert Wavefront *.obj models "
              "into Vangers *.m3d and *.a3d models."
            "\nFor each game directory found in \"source_dir\" progam "
                "creates output directory in \"output_dir\"."
            "\n"
            "\nEach directory with multiple wavefront *.obj files "
                "is converted into single *.m3d or *.a3d file."
            "\n"
            "\nUse \"weapon_attachment_point_file\" option to specify path "
                "to wavefront *.obj file "
                "which was used while converting to *.obj."
            "\nUse \"center_of_mass_file\" option to specify path "
                "to wavefront *.obj file "
                "which was used while converting to *.obj."
            "\nUse \"c3d_default_material\" option to specify "
                "default material for polygons "
                "which have no material or unexpected one."
            "\n"
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tcreate_wavefront_mtl - Create *.mtl file for *.obj files "
              "generated with \"vangers_3d_model_to_obj\"."
            "\nWorlds' *.pal files are expected as input."
            "\nThey should be supplied with this program."
            "\n"
            "\nFor each *.pal file found in \"source_dir\" "
                "*.mtl file is generated in \"output_dir\"."
            "\n"
            "\nSpecify \"3d_obj_float_precision\" option to change "
                "precision of float numbers of output *.mtl files."
            "\nSpecify \"mtl_n_wheels\" option to set "
                "number of generated wheel materials."
            "\nSpecify \"mtl_body_offs\" option to add "
                "additional body materials."
            "\n"
            "\n\"source_dir\" and \"output_dir\" options must be specified."
          "\n"
          "\n"
          "\n"
          "\n\tcreate_materials_table - Create *.html table file "
              "which shows exact material colors."
            "\nWorlds' *.pal files are expected as input."
            "\nThey should be supplied with this program."
            "\n"
            "\nFor each *.pal file found in \"source_dir\" "
                "*.html file is generated in \"output_dir\"."
            "\n"
            "\nSpecify \"mtl_body_offs\" option to add "
                "additional body materials."
            "\n"
            "\n\"source_dir\" and \"output_dir\" options must be specified."
        )
      )
      ("source_dir,s",
       boost::program_options::value<std::string>(),
       "\tDirectory with files to input.\n")
      ("source_file,f",
       boost::program_options::value<std::string>(),
       "\tFile to input.\n")
      ("output_dir,d",
       boost::program_options::value<std::string>(),
       "\tDirectory where to output.\n")
      ("output_file,o",
       boost::program_options::value<std::string>(),
       "\tFile where to output.\n")
      ("readable_output,r",
       boost::program_options::bool_switch()->default_value(false),
       "\tGenerate human readable output instead of one usable by programs.\n"
       "\tUsed by \"usage_pal\" and \"compare_bmp_escave_outside\" modes.\n")
      ("reversed",
       boost::program_options::bool_switch()->default_value(false),
       "\tConvert other way around.\n"
       "\tUsed by \"vangers_pal_to_tga_pal\" mode.\n")

      ("usage_pal_for_each_file",
       boost::program_options::bool_switch()->default_value(false),
       "\tCreate usage palette for each *.bmp file "
           "instead of just one for all.\n"
       "\tUsed by \"usage_pal\" mode.\n")
      ("output_dir_unused",
       boost::program_options::value<std::string>(),
       "\tWhere to output files with unused colors.\n"
       "\tUsed by \"remove_not_used_pal\".\n")
      ("usage_pals_dir",
       boost::program_options::value<std::string>(),
       "\tWhere to search for usage palettes.\n"
       "\tUsed by \"remove_not_used_pal\" mode.\n")
      ("unused_pals_dir",
       boost::program_options::value<std::string>(),
       "\tWhere to search for unused colors palettes.\n"
       "\tUsed by \"tga_merge_unused_pal\" mode.\n")
      ("pal_for_each_file",
       boost::program_options::bool_switch()->default_value(false),
       "\tWhether to use single palette specified by \"pal\" option "
           "or folder with palette for each file "
           "specified by \"pal_dir\" option.\n"
       "\tUsed by \"bmp_to_tga\" mode.\n")
      ("pal",
       boost::program_options::value<std::string>(),
       "\tColor palette to form *.tga files from Vangers *.bmp ones.\n"
       "\tUsed by \"bmp_to_tga\" mode.\n")
      ("pal_dir",
       boost::program_options::value<std::string>(),
       "\tDirectory with color palettes for each input file.\n"
       "\tUsed by \"bmp_to_tga\" and \"tga_replace_pal\" modes.\n")
      ("dir_to_compare",
       boost::program_options::value<std::string>(),
       "\tDirectory with *.bmp files to compare against source directory.\n"
       "\tUsed by \"compare_bmp_escave_outside\" mode.\n")
      ("items_bmp",
       boost::program_options::bool_switch()->default_value(false),
       "\tConverting item files so 2 *.bmp "
           "should be created for each *.tga file.\n"
       "\"map\" and \"output_dir_through_map\" "
           "must be specified in this case.\n"
       "\tUsed by \"tga_to_bmp\" mode.\n")
      ("map",
       boost::program_options::value<std::string>(),
       "\tPath to \"compare_bmp_escave_outside\" mode output to use.\n"
       "\tUsed by \"tga_to_bmp\" mode.\n")
      ("output_dir_through_map",
       boost::program_options::value<std::string>(),
       "\tDirectory where to output *.bmp files "
         "generated with \"compare_bmp_escave_outside\" map.\n"
       "\tUsed by \"tga_to_bmp\" mode.\n")
      ("fix_null_bytes_and_direction",
       boost::program_options::bool_switch()->default_value(false),
       "\tIf null bytes got changed to never used color "
           "or image got rotated/flipped "
           "specify this option to deal with those problems.\n"
       "\tUsed by \"tga_to_bmp\" mode.\n")

      ("3d_obj_float_precision",
       boost::program_options::value<int>()->default_value(6),
       "\tPrecision of float numbers of output Wavefront object "
           "and material files.\n"
       "\tDefaults to 6.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" "
       "and \"create_wavefront_mtl\" modes.\n")
      ("3d_default_scale",
       boost::program_options::value<double>()->
         default_value(TRACTOR_CONVERTER_DEFAULT_3D_DEFAULT_SCALE),
       "\tIf there is no info about scale_size for some object "
           "in *.prm or game.lst configs this valuse is used.\n"
       "\tDefaults to " TRACTOR_CONVERTER_DEFAULT_3D_DEFAULT_SCALE_STR ".\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("m3d_weapon_file",
       boost::program_options::value<std::string>()->default_value(""),
       "\tName of *.m3d file which contains weapon model.\n"
       "\tThis model is used to indicate positions "
           "of weapons slots on mechos models.\n"
       "\tDefaults to empty string.\n"
       "\tUsed by \"vangers_3d_model_to_obj\".\n")
      ("weapon_attachment_point_file",
       boost::program_options::value<std::string>()->default_value(""),
       "\tPath to wavefront *.obj file.\n"
       "\tThis *.obj model is used to indicate position of attachment point "
           "in weapons models.\n"
       "\tDefaults to empty string.\n"
       "\tUsed by \"vangers_3d_model_to_obj\", "
           "\"obj_to_vangers_3d_model\" mode.\n")
      ("ghost_wheel_file",
       boost::program_options::value<std::string>()->default_value(""),
       "\tPath to wavefront *.obj file.\n"
       "\tThis *.obj model is used to indicate "
           "position of wheels with no polygons.\n"
       "\tDefaults to empty string.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("extract_center_of_mass",
       boost::program_options::bool_switch()->default_value(false),
       "\tExtract center of mass from *.m3d/a3d file and mark it.\n"
       "\tDefaults to false.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("center_of_mass_file",
       boost::program_options::value<std::string>()->default_value(""),
       "\tPath to wavefront *.obj file.\n"
       "\tThis *.obj model is used to indicate position "
           "of extracted center of mass.\n"
       "\tDefaults to empty string.\n"
       "\tUsed by \"vangers_3d_model_to_obj\", "
           "\"obj_to_vangers_3d_model\" mode.\n")
      ("wavefront_mtl",
       boost::program_options::value<std::string>()->default_value(""),
       "\tPath to *.mtl file for generated *.obj files.\n"
       "\tCopied to output directories.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("extract_nonexistent_weapons",
       boost::program_options::bool_switch()->default_value(false),
       "\tExtract weapons even when they are marked as nonexistent.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("use_custom_volume_by_default",
       boost::program_options::bool_switch()->default_value(false),
       "\tGenerate per-file configs with uncommented "
           "custom volume options.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("use_custom_rcm_by_default",
       boost::program_options::bool_switch()->default_value(false),
       "\tGenerate per-file configs with uncommented "
           "custom center of mass options.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("use_custom_J_by_default",
       boost::program_options::bool_switch()->default_value(false),
       "\tGenerate per-file configs with uncommented "
           "custom inertia tensor options.\n"
       "\tUsed by \"vangers_3d_model_to_obj\" mode.\n")
      ("c3d_default_material",
       boost::program_options::value<std::string>()->
         default_value(TRACTOR_CONVERTER_DEFAULT_C3D_DEFAULT_MATERIAL),
       "\tName of default material.\n"
       "\tIf polygon have unexpected material or no material "
           "then default material is assigned to it.\n"
       "\tDefaults to " TRACTOR_CONVERTER_DEFAULT_C3D_DEFAULT_MATERIAL ".\n"
       "\tUsed by \"obj_to_vangers_3d_model\" mode.\n")
      ("mtl_n_wheels",
       boost::program_options::value<std::size_t>()->default_value(50),
       "\tNumber of wheel materials to generate for *.mtl file.\n"
       "\tDefaults to 50.\n"
       "\tUsed by \"create_wavefront_mtl\" mode.\n")
      ("mtl_body_offs",
       boost::program_options::value<std::vector<std::string>>(),
       "\tBody materials to create.\n"
       "\tMust be 2 integers delimited by any non-numeric character.\n"
       "\tExample config file entry:\n"
       "\tmtl_body_offs = 129_3\n"
       "\tOption may be specified multiple times for multiple materials.\n"
       "\tExample config file entry:\n"
       "\tmtl_body_offs = 129_3\n"
       "\tmtl_body_offs = 130_3\n"
       "\tMultiple body materials may be specified for one option.\n"
       "\tThey must be delimited by any non-numeric character.\n"
       "\tExample config file entry:\n"
       "\tmtl_body_offs = 129_3;130_3\n"
       "\tUsed by \"create_wavefront_mtl\" and \"create_materials_table\" modes.\n")
      ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    boost::program_options::options_description hidden("Hidden options");
//  hidden.add_options()
//    ("input-file",
//     boost::program_options::value<std::vector<std::string>>(),
//     "input file")
//    ;

    boost::program_options::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    boost::program_options::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    boost::program_options::options_description visible("Allowed options");
    visible.add(generic).add(config);

    boost::program_options::positional_options_description p;
    p.add("source_dir", -1);

    boost::program_options::store(
      boost::program_options::command_line_parser(ac, av).
        options(cmdline_options).positional(p).run(),
      vm);
    boost::program_options::notify(vm);


//  if(vm.count("help"))
    if(helpers::check_option(vm,
                             "help",
                             TRACTOR_CONVERTER_DONT_THROW_ON_FAILURE))
    {
      std::cout << visible << "\n";
      std::exit(EXIT_SUCCESS);
    }

//  if(vm.count("version"))
    if(helpers::check_option(vm,
                             "version",
                             TRACTOR_CONVERTER_DONT_THROW_ON_FAILURE))
    {
      std::cout << "Tractor converter program, version " <<
        TRACTOR_CONVERTER_VERSION << "\n";
      std::exit(EXIT_SUCCESS);
    }


    std::ifstream ifs(config_file.c_str());
    if(!ifs)
    {
      std::cout << "can not open config file: " << config_file << "\n";
    }
    else
    {
      boost::program_options::store(
        parse_config_file(ifs, config_file_options),
        vm);
      boost::program_options::notify(vm);
    }
  }
  catch(std::exception &)
  {
    std::cout << "failed to get command line options" << '\n';
    throw;
  }
  return vm;
}



} // namespace tractor_converter
