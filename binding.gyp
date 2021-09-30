{
  'targets': [
    {
      'target_name': 'info',
      'win_delay_load_hook': 'false',
      'sources': [
        'src/info.cc',
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
