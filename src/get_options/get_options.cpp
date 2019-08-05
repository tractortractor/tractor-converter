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
      ((option::name::version + ",v").c_str(), "\tPrint version.\n")
      ((option::name::help + ",h").c_str(), "\tProduce help message.\n")
      ((option::name::config + ",c").c_str(),
       boost::program_options::value<std::string>(&config_file)->
         default_value(option::default_val::config),
       "\tPath to the configuration file.\n")
      ;

    // Declare a group of options that will be
    // allowed both on command line and in
    // config file.
    boost::program_options::options_description config("Configuration");
    config.add_options()
      ((option::name::mode + ",m").c_str(),
       boost::program_options::value<std::string>(),
       ((
         "\tMode of operation."
         "\nPossible values are:"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n\tusage_pal - Create black-and-white color palette to indicate "
               "which colors are used for Vangers *.bmp files in " +
               option::name::source_dir + "."
             "\n"
             "\nBy default, creates one usage palette for all files."
             "\n\"" + option::name::output_file + "\" "
                 "must be specified in this case."
             "\n"
             "\nIf \"" + option::name::usage_pal_for_each_file + "\" "
                 "is specified, creates usage palette for each file."
             "\n\"" + option::name::output_dir + "\" "
                 "must be specified in this case."
             "\n"
             "\n\"" + option::name::source_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tremove_not_used_pal - Use output of \"usage\" mode "
               "to create 2 palettes."
             "\n"
             "\nOne with used colors goes to "
                 "\"" + option::name::output_dir + "\"."
             "\n"
             "\nOne with unused colors goes to "
                 "\"" + option::name::output_dir_unused + "\"."
             "\nThose unused colors are still needed to display text "
                 "and other game elements properly."
             "\nAfter performing all changes, you should merge unused "
                 "palette back to *.tga image with " +
                 mode::name::tga_merge_unused_pal + " mode."
             "\n"
             "\n\"" + option::name::source_dir + "\", "
                 "\"" + option::name::output_dir + "\", "
                 "\"" + option::name::output_dir_unused + "\" and "
                 "\"" + option::name::usage_pals_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\ttga_merge_unused_pal - Merge palette of *.tga file "
               "with unused colors palette."
             "\n\"" + option::name::source_dir + "\", "
                 "\"" + option::name::output_dir + "\" and "
                 "\"" + option::name::unused_pals_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\ttga_replace_pal - Replace palette of *.tga file "
               "with color palette from \"" + option::name::pal_dir + "\"."
             "\n\"" + option::name::source_dir + "\", "
                 "\"" + option::name::output_dir + "\" and "
                 "\"" + option::name::pal_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\textract_tga_pal - Get raw palettes from *.tga files."
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tvangers_pal_to_tga_pal - Convert Vangers palettes "
               "to normal *.tga ones."
             "\nPalette files in \"" + option::name::source_dir + "\" "
                 "must have *.pal extension."
             "\nSpecify \"" + option::name::reversed + "\" "
                 "option to convert *.tga palettes to Vangers ones."
             "\n\"" + option::name::source_dir + "\" "
                 "and \"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tpal_shift_for_vangers_avi - "
               "Move colors from 0-127 to 128-255, "
               "overwrite 0-127 with zero colors."
             "\nPalette files in \"" + option::name::source_dir + "\" "
                 "must have *.pal extension."
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tcompare_bmp_escave_outside - Compare values of bytes "
               "in *.bmp images for escave and non-escave files."
             "\nCreate map to indicate which bytes of source files "
                 "match bytes of compared files."
             "\nSpecify \"" + option::name::readable_output + "\" option "
                 "to get human readable output."
             "\n\"" + option::name::source_dir + "\", "
                 "\"" + option::name::dir_to_compare + "\" and "
                 "\"" + option::name::output_file + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tbmp_to_tga - Convert Vangers *.bmp files "
               "from " + option::name::source_dir + " folder to *.tga files."
             "\n"
             "\nAll input *.pal files must be converted from Vangers format "
                 "to normal one with "
                 "\"" + mode::name::vangers_pal_to_tga_pal + "\" mode."
             "\n"
             "\nBy default, \"" + option::name::pal + "\" "
                 "option must be used to specify "
                 "single palette to use for all *.bmp files."
             "\n"
             "\nIf \"" + option::name::pal_for_each_file + "\" "
                 "option is specified, \"" + option::name::pal_dir + "\" "
                 "must be specified instead of \"" + option::name::pal + "\"."
             "\nIn this case, for each *.bmp file in "
                 "\"" + option::name::source_dir + "\", "
                 "there must be *.pal file with same name in "
                 "\"" + option::name::pal_dir + "\"."
             "\n"
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\ttga_to_bmp - Convert *.tga images to Vangers *.bmp ones."
             "\nRLE compression is not supported."
             "\n"
             "\nSpecify \"" + option::name::items_bmp + "\" "
                 "option to convert item files."
             "\nThere are 2 different images for each item: "
                 "one used inside escave and one used outside."
             "\nOutput of \"" + mode::name::cmp_bmp_escave_outside + "\" "
                 "mode should be used to create those 2 images "
                 "from each *.tga file."
             "\n"
             "\nSpecify "
                 "\"" + option::name::fix_null_bytes_and_direction + "\" "
                 "option in case transparency null bytes were changed "
                 "to never used color or image got rotated/flipped."
             "\n"
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
             "\n\"" + option::name::map + "\" and "
                 "\"" + option::name::output_dir_through_map + "\" "
                 "options must be specified while converting items."
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n\tvangers_3d_model_to_obj - Convert Vangers *.m3d and *.a3d "
               "models into Wavefront *.obj ones."
             "\nFor each game directory found in "
                 "\"" + option::name::source_dir + "\", output directory in "
                 "\"" + option::name::output_dir + "\" is created."
             "\n"
             "\nEach *.m3d/*.a3d file contains multiple 3d objects."
             "\nFor each *.m3d/*.a3d file, folder with all enclosed models "
                 "in Wavefront object format is created."
             "\n"
             "\nSpecify \"" + option::name::obj_float_precision + "\" "
                 "option to change precision "
                 "of float numbers of output *.obj files."
             "\nSpecify \"" + option::name::default_scale + "\" "
                 "option to set scale for objects "
                 "for which there is no scale info "
                 "in *.prm and game.lst files."
             "\nUse \"" + option::name::m3d_weapon_file + "\" "
                 "option to specify name of *.m3d "
                 "weapon model to insert into mechos models "
                 "to indicate positions of weapons."
             "\nUse \"" + option::name::weapon_attachment_point_file + "\" "
                 "option to specify path to Wavefront *.obj file which "
                 "contains model to be inserted into weapon model to "
                 "indicate weapon attachment point."
             "\nUse \"" + option::name::ghost_wheel_file + "\" option "
                 "to specify path to Wavefront *.obj file which contains "
                 "model to be inserted in place of wheels with no polygons."
             "\nSpecify \"" + option::name::extract_bound_model + "\" "
                 "option to extract bound models of *.m3d files."
             "\nSpecify \"" + option::name::extract_center_of_mass + "\" "
                 "option to mark center of mass found in *.m3d/*.a3d file."
             "\nUse \"" + option::name::center_of_mass_file + "\" "
                 "to specify path to Wavefront *.obj file "
                 "which contains model which will mark center of mass."
             "\nSpecify \"" + option::name::wavefront_mtl + "\" "
                 "option to copy *.mtl file to output folders "
                 "so generated *.obj files will refer to it."
             "\nSpecify \"" + option::name::extract_nonexistent_weapons + "\" "
                 "option to extract weapons which are marked as nonexistent."
             "\nSpecify "
                 "\"" + option::name::use_custom_volume_by_default + "\" "
                 "to enable by default custom volume "
                 "option in generated per-file configs."
             "\nSpecify \"" + option::name::use_custom_rcm_by_default + "\" "
                 "to enable by default custom center of mass option "
                 "in generated per-file configs."
             "\nSpecify \"" + option::name::use_custom_J_by_default + "\" "
                 "to enable by default custom inertia tensor "
                 "option in generated per-file configs."
             "\n"
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tobj_to_vangers_3d_model - Convert Wavefront *.obj models "
               "into Vangers *.m3d and *.a3d models."
             "\nFor each game directory found in "
                 "\"" + option::name::source_dir + "\", this mode creates "
                 "output directory in \"" + option::name::output_dir + "\"."
             "\n"
             "\nEach directory with multiple Wavefront *.obj files "
                 "is converted into single *.m3d or *.a3d file."
             "\n"
             "\nUse \"" + option::name::weapon_attachment_point_file + "\" "
                 "option to specify path to Wavefront *.obj file "
                 "which was used while converting to *.obj."
             "\nUse \"" + option::name::center_of_mass_file + "\" "
                 "option to specify path to Wavefront *.obj file "
                 "which was used while converting to *.obj."
             "\nUse \"" + option::name::default_c3d_material + "\" "
                 "option to specify default material for polygons "
                 "which have no material or unexpected one."
             "\nUse \"" + option::name::scale_cap + "\" "
                 "option to specify maximum scale."
             "\nSpecify \"" + option::name::center_model + "\" option "
                 "to automatically move model to center of extreme points."
             "\nSpecify \"" + option::name::recalculate_vertex_normals + "\" "
                 "option to recalculate vertex normals."
             "\nUse \"" + option::name::max_smooth_angle + "\" "
                 "option to specify max angle between "
                 "smooth faces when generating normals."
             "\nSpecify \"" + option::name::gen_bound_models + "\" "
                 "option to automatically generate bound models."
             "\n\"" + option::name::gen_bound_layers_num + "\" "
                 "option is used while generating bound models."
             "\n\"" + option::name::gen_bound_area_threshold + "\" "
                 "option is used while generating bound models."
             "\n"
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tcreate_wavefront_mtl - Create *.mtl file for *.obj files "
               "generated with "
               "\"" + mode::name::vangers_3d_model_to_obj + "\"."
             "\nWorlds' *.pal files are expected as input."
             "\nThey should be supplied with this program."
             "\n"
             "\nFor each *.pal file found in "
                 "\"" + option::name::source_dir + "\", "
                 "*.mtl file is generated in "
                 "\"" + option::name::output_dir + "\"."
             "\n"
             "\nSpecify \"" + option::name::obj_float_precision + "\" "
                 "option to change precision "
                 "of float numbers of output *.mtl files."
             "\nSpecify \"" + option::name::mtl_n_wheels + "\" "
                 "option to set number of generated wheel materials."
             "\nSpecify \"" + option::name::mtl_body_offs + "\" "
                 "option to add additional body materials."
             "\n"
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
           "\n"
           "\n"
           "\n"
           "\n\tcreate_materials_table - Create *.html table file "
               "which shows exact material colors."
             "\nWorlds' *.pal files are expected as input."
             "\nThey should be supplied with this program."
             "\n"
             "\nFor each *.pal file found in "
                 "\"" + option::name::source_dir + "\", "
                 "*.html file is generated in "
                 "\"" + option::name::output_dir + "\"."
             "\n"
             "\nSpecify \"" + option::name::mtl_body_offs + "\" "
                 "option to add additional body materials."
             "\n"
             "\n\"" + option::name::source_dir + "\" and "
                 "\"" + option::name::output_dir + "\" "
                 "options must be specified."
        ).c_str())
      )
      ((option::name::source_dir + ",s").c_str(),
       boost::program_options::value<std::string>(),
       "\tDirectory with files to input.\n")
      ((option::name::source_file + ",f").c_str(),
       boost::program_options::value<std::string>(),
       "\tFile to input.\n")
      ((option::name::output_dir + ",d").c_str(),
       boost::program_options::value<std::string>(),
       "\tDirectory where to output.\n")
      ((option::name::output_file + ",o").c_str(),
       boost::program_options::value<std::string>(),
       "\tFile where to output.\n")

      ((option::name::readable_output + ",r").c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::readable_output),
       ("\tGenerate human readable output instead of one usable by programs.\n"
        "\tUsed by \"" + mode::name::usage_pal + "\" and "
            "\"" + mode::name::cmp_bmp_escave_outside + "\" "
            "modes.\n").c_str())
      (option::name::reversed.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::reversed),
       ("\tConvert other way around.\n"
        "\tUsed by \"" + mode::name::vangers_pal_to_tga_pal + "\" "
            "mode.\n").c_str())
      (option::name::usage_pal_for_each_file.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::usage_pal_for_each_file),
       ("\tCreate usage palette for each *.bmp file "
            "instead of just one for all.\n"
        "\tUsed by \"" + mode::name::usage_pal + "\" mode.\n").c_str())
      (option::name::output_dir_unused.c_str(),
       boost::program_options::value<std::string>(),
       ("\tWhere to output files with unused colors.\n"
        "\tUsed by \"" + mode::name::remove_not_used_pal + "\".\n").c_str())
      (option::name::usage_pals_dir.c_str(),
       boost::program_options::value<std::string>(),
       ("\tWhere to search for usage palettes.\n"
        "\tUsed by \"" + mode::name::remove_not_used_pal + "\" "
            "mode.\n").c_str())
      (option::name::unused_pals_dir.c_str(),
       boost::program_options::value<std::string>(),
       ("\tWhere to search for unused colors palettes.\n"
        "\tUsed by \"" + mode::name::tga_merge_unused_pal + "\" "
            "mode.\n").c_str())
      (option::name::pal_for_each_file.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::pal_for_each_file),
       ("\tWhether to use single palette specified by "
            "\"" + option::name::pal + "\" option "
            "or folder with palette for each file "
            "specified by \"" + option::name::pal_dir + "\" option.\n"
        "\tUsed by \"" + mode::name::bmp_to_tga + "\" mode.\n").c_str())
      (option::name::pal.c_str(),
       boost::program_options::value<std::string>(),
       ("\tColor palette to form *.tga files from Vangers *.bmp ones.\n"
        "\tUsed by \"" + mode::name::bmp_to_tga + "\" mode.\n").c_str())
      (option::name::pal_dir.c_str(),
       boost::program_options::value<std::string>(),
       ("\tDirectory with color palettes for each input file.\n"
        "\tUsed by \"" + mode::name::bmp_to_tga + "\" and "
            "\"" + mode::name::tga_replace_pal + "\" modes.\n").c_str())
      (option::name::dir_to_compare.c_str(),
       boost::program_options::value<std::string>(),
       ("\tDirectory with *.bmp files to compare against source directory.\n"
        "\tUsed by \"" + mode::name::cmp_bmp_escave_outside + "\" "
            "mode.\n").c_str())
      (option::name::items_bmp.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::items_bmp),
       ("\tConverting item files so 2 *.bmp "
            "should be created for each *.tga file.\n"
        "\t\"" + option::name::map + "\" and "
            "\"" + option::name::output_dir_through_map + "\" "
            "must be specified in this case.\n"
        "\tUsed by \"" + mode::name::tga_to_bmp + "\" mode.\n").c_str())
      (option::name::map.c_str(),
       boost::program_options::value<std::string>(),
       ("\tPath to \"" + mode::name::cmp_bmp_escave_outside + "\" "
            "mode output to use.\n"
        "\tUsed by \"" + mode::name::tga_to_bmp + "\" mode.\n").c_str())
      (option::name::output_dir_through_map.c_str(),
       boost::program_options::value<std::string>(),
       ("\tDirectory where to output *.bmp files generated with "
            "\"" + mode::name::cmp_bmp_escave_outside + "\" map.\n"
        "\tUsed by \"" + mode::name::tga_to_bmp + "\" mode.\n").c_str())
      (option::name::fix_null_bytes_and_direction.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::fix_null_bytes_and_direction),
       ("\tIf null bytes got changed to never used color "
            "or image got rotated/flipped, "
            "specify this option to deal with those problems.\n"
        "\tUsed by \"" + mode::name::tga_to_bmp + "\" mode.\n").c_str())

      (option::name::obj_float_precision.c_str(),
       boost::program_options::value<int>()->
         default_value(option::default_val::obj_float_precision),
       ("\tPrecision of float numbers of output Wavefront object "
            "and material files.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" and "
            "\"" + mode::name::create_wavefront_mtl + "\" modes.\n").c_str())
      (option::name::default_scale.c_str(),
       boost::program_options::value<double>()->
         default_value(option::default_val::default_scale),
       ("\tIf there is no info about scale_size for some object "
            "in *.prm or game.lst configs, this value is used.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::m3d_weapon_file.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::m3d_weapon_file),
       ("\tName of *.m3d file which contains weapon model.\n"
        "\tThis model is used to indicate positions "
            "of weapons slots on mechos models.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::weapon_attachment_point_file.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::weapon_attachment_point_file),
       ("\tPath to Wavefront *.obj file.\n"
        "\tThis *.obj model is used to indicate position of attachment point "
            "in weapons models.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" and "
            "\"" + mode::name::obj_to_vangers_3d_model + "\" "
            "modes.\n").c_str())
      (option::name::ghost_wheel_file.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::ghost_wheel_file),
       ("\tPath to Wavefront *.obj file.\n"
        "\tThis *.obj model is used to indicate "
            "position of wheels with no polygons.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::extract_bound_model.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::extract_bound_model),
       ("\tExtract bound models from *.m3d files.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::extract_center_of_mass.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::extract_center_of_mass),
       ("\tExtract center of mass from *.m3d/*.a3d file and mark it.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::center_of_mass_file.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::center_of_mass_file),
       ("\tPath to Wavefront *.obj file.\n"
        "\tThis *.obj model is used to indicate position "
            "of extracted center of mass.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" and "
            "\"" + mode::name::obj_to_vangers_3d_model + "\" "
            "modes.\n").c_str())
      (option::name::wavefront_mtl.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::wavefront_mtl),
       ("\tPath to *.mtl file for generated *.obj files.\n"
        "\tCopied to output directories.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::extract_nonexistent_weapons.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::extract_nonexistent_weapons),
       ("\tExtract weapons even when they are marked as nonexistent.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::use_custom_volume_by_default.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::use_custom_volume_by_default),
       ("\tGenerate per-file configs with uncommented "
            "custom volume options.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::use_custom_rcm_by_default.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::use_custom_rcm_by_default),
       ("\tGenerate per-file configs with uncommented "
            "custom center of mass options.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::use_custom_J_by_default.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::use_custom_J_by_default),
       ("\tGenerate per-file configs with uncommented "
            "custom inertia tensor options.\n"
        "\tUsed by \"" + mode::name::vangers_3d_model_to_obj + "\" "
            "mode.\n").c_str())
      (option::name::default_c3d_material.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::default_c3d_material),
       ("\tName of default material.\n"
        "\tIf polygon has unexpected material or no material, "
            "then default material is assigned to it.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::scale_cap.c_str(),
       boost::program_options::value<double>()->
         default_value(option::default_val::scale_cap),
       ("\tIf model scale is higher than this cap, "
            "it will be lowered to this cap.\n"
        "\tNeeded since the game can't properly render objects "
            "beyond certain scale_size under certain circumstances.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::center_model.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::center_model),
       ("\tAutomatically move model to center of extreme points.\n"
        "\tShould be always turned on to ensure proper position of "
            "in-game xyzmax bounding box and rmax bounding sphere.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::recalculate_vertex_normals.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::recalculate_vertex_normals),
       ("\tRecalculate vertex normals.\n"
        "\tShould be always turned on unless "
            "vertex normals were manually set specifically for this game.\n"
        "\tBad vertex normals will most likely result in "
            "polygons appearing black when should be bright and vice versa.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::max_smooth_angle.c_str(),
       boost::program_options::value<std::string>()->
         default_value(option::default_val::max_smooth_angle),
       ("\tUsed when recalculating vertex normals.\n"
        "\tAngle is measured in radians by default.\n"
        "\tIf \"d\" character is found right after the number, "
            "value is measured in degrees.\n"
        "\tIf angle between normals of two connected polygons "
            "is lower than this value, they will be considered smooth "
            "relative to each other.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::gen_bound_models.c_str(),
       boost::program_options::bool_switch()->
         default_value(option::default_val::gen_bound_models),
       ("\tAutomatically generate bound models.\n"
        "\tShould be always turned on unless "
            "bound models were manually edited.\n"
        "\t*.obj bound models are ignored if turned on.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::gen_bound_layers_num.c_str(),
       boost::program_options::value<std::size_t>()->
         default_value(option::default_val::gen_bound_layers_num),
       ("\tUsed when generating bound model.\n"
        "\tMore layers will make generated bound model more precise.\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::gen_bound_area_threshold.c_str(),
       boost::program_options::value<double>()->
         default_value(option::default_val::gen_bound_area_threshold),
       ("\tUsed when generating bound model.\n"
        "\tWith higher values more parts of original model will be considered "
            "insignificant and will end up outside of generated bound.\n"
        "\tMaximum is " +
            option::max::gen_bound_area_threshold_str + ".\n"
        "\tUsed by \"" + mode::name::obj_to_vangers_3d_model + "\" "
            "mode.\n").c_str())
      (option::name::mtl_n_wheels.c_str(),
       boost::program_options::value<std::size_t>()->
         default_value(option::default_val::mtl_n_wheels),
       ("\tNumber of wheel materials to generate for *.mtl file.\n"
        "\tUsed by \"" + mode::name::create_wavefront_mtl + "\" "
            "mode.\n").c_str())
      (option::name::mtl_body_offs.c_str(),
       boost::program_options::value<std::vector<std::string>>(),
       ("\tBody materials to create.\n"
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
        "\tUsed by \"" + mode::name::create_wavefront_mtl + "\" and "
            "\"" + mode::name::create_materials_table + "\" modes.\n").c_str())
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
    p.add(option::name::source_dir.c_str(), -1);

    boost::program_options::store(
      boost::program_options::command_line_parser(ac, av).
        options(cmdline_options).positional(p).run(),
      vm);
    boost::program_options::notify(vm);


    if(helpers::check_option(vm,
                             option::name::help,
                             error_handling::none))
    {
      std::cout << visible << "\n";
      std::exit(EXIT_SUCCESS);
    }

    if(helpers::check_option(vm,
                             option::name::version,
                             error_handling::none))
    {
      std::cout << "Tractor converter program, version " <<
        define::version << "\n";
      std::exit(EXIT_SUCCESS);
    }


    std::ifstream ifs(config_file.c_str());
    if(!ifs)
    {
      std::cout << "Failed to open config file: " << config_file << "\n";
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
    std::cout << "Failed to get command line options." << '\n';
    throw;
  }
  return vm;
}



} // namespace tractor_converter
