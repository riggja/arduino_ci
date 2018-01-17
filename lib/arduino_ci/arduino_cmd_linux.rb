require 'arduino_ci/arduino_cmd'
require 'arduino_ci/display_manager'

module ArduinoCI

  # Implementation of Arduino linux IDE commands
  class ArduinoCmdLinux < ArduinoCmd

    attr_reader :prefs_response_time

    flag :get_pref,        "--get-pref"
    flag :set_pref,        "--pref"
    flag :save_prefs,      "--save-prefs"
    flag :use_board,       "--board"
    flag :install_boards,  "--install-boards"
    flag :install_library, "--install-library"
    flag :verify,          "--verify"

    def initialize
      super
      @prefs_response_time = nil
      @display_mgr         = DisplayManager::instance
    end

    # fetch preferences to a hash
    def _prefs_raw
      start = Time.now
      resp = run_and_capture(flag_get_pref)
      @prefs_response_time = Time.now - start
      return nil unless resp[:success]
      resp[:out]
    end

    def _lib_dir
      File.join(get_pref("sketchbook.path"), "libraries")
    end

    # run the arduino command
    def run(*args, **kwargs)
      full_args = @base_cmd + args
      @display_mgr.run(*full_args, **kwargs)
    end

    def run_with_gui_guess(message, *args, **kwargs)
      # On Travis CI, we get an error message in the GUI instead of on STDERR
      # so, assume that if we don't get a rapid reply that things are not installed

      prefs if @prefs_response_time.nil?
      x3 = @prefs_response_time * 3
      Timeout.timeout(x3) do
        result = run_and_capture(*args, **kwargs)
        result[:success]
      end
    rescue Timeout::Error
      puts "No response in #{x3} seconds. Assuming graphical modal error message#{message}."
      false
    end

    # underlying preference-setter.
    # @param key [String] the preference key
    # @param value [String] the preference value
    # @return [bool] whether the command succeeded
    def _set_pref(key, value)
      run_with_gui_guess(" about preferences", flag_set_pref, "#{key}=#{value}", flag_save_prefs)
    end

    # check whether a board is installed
    # we do this by just selecting a board.
    #   the arduino binary will error if unrecognized and do a successful no-op if it's installed
    def board_installed?(boardname)
      run_with_gui_guess(" about board not installed", flag_use_board, boardname)
    end

    # use a particular board for compilation
    def use_board(boardname)
      run_with_gui_guess(" about board not installed", flag_use_board, boardname, flag_save_prefs)
    end

  end

end
