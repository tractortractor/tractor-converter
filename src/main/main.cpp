#include "main.hpp"



int main(int argc, char** argv)
{

  try
  {
    const boost::program_options::variables_map options =
      tractor_converter::get_options(argc, argv);

    tractor_converter::helpers::check_option(
      options,
      tractor_converter::option::name::mode);

    const std::string current_mode =
      options[tractor_converter::option::name::mode].as<std::string>();
    if(current_mode == tractor_converter::mode::name::usage_pal)
    {
      tractor_converter::usage_pal_mode(options);
    }
    else if(current_mode == tractor_converter::mode::name::remove_not_used_pal)
    {
      tractor_converter::remove_not_used_pal_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::tga_merge_unused_pal)
    {
      tractor_converter::tga_merge_unused_pal_mode(options);
    }
    else if(current_mode == tractor_converter::mode::name::tga_replace_pal)
    {
      tractor_converter::tga_replace_pal_mode(options);
    }
    else if(current_mode == tractor_converter::mode::name::extract_tga_pal)
    {
      tractor_converter::extract_tga_pal_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::vangers_pal_to_tga_pal)
    {
      tractor_converter::vangers_pal_to_tga_pal_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::pal_shift_for_vangers_avi)
    {
      tractor_converter::pal_shift_for_vangers_avi_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::cmp_bmp_escave_outside)
    {
      tractor_converter::compare_bmp_escave_outside_mode(options);
    }
    else if(current_mode == tractor_converter::mode::name::bmp_to_tga)
    {
      tractor_converter::bmp_to_tga_mode(options);
    }
    else if(current_mode == tractor_converter::mode::name::tga_to_bmp)
    {
      tractor_converter::tga_to_bmp_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::vangers_3d_model_to_obj)
    {
      tractor_converter::vangers_3d_model_to_obj_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::obj_to_vangers_3d_model)
    {
      tractor_converter::obj_to_vangers_3d_model_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::create_wavefront_mtl)
    {
      tractor_converter::create_wavefront_mtl_mode(options);
    }
    else if(current_mode ==
            tractor_converter::mode::name::create_materials_table)
    {
      tractor_converter::create_materials_table_mode(options);
    }
    else
    {
      throw std::runtime_error(
        "Current mode \"" + current_mode + "\" is unknown.");
    }
    return EXIT_SUCCESS;
  }
  catch(boost::program_options::error &e)
  {
    std::cout << "tractor_converter failed: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  catch(boost::filesystem::filesystem_error &e)
  {
    std::cout << "tractor_converter failed: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  catch(std::exception &e)
  {
    std::cout << "tractor_converter failed: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
}
