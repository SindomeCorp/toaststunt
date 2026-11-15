require 'test_helper'
require 'json'

class TestVerbCounts < Test::Unit::TestCase

  def test_that_only_wizards_can_clear_verb_counts
    run_test_as('programmer') do
      assert_equal E_PERM, simplify(command('; return clear_verb_counts();'))
    end
    run_test_as('wizard') do
      assert_equal 0, simplify(command('; return clear_verb_counts();'))
      assert_equal E_ARGS, simplify(command('; return clear_verb_counts(1);'))
    end
  end

  def test_that_reset_publishes_empty_counts_and_new_calls_start_at_one
    log_path = ENV['TOAST_VERB_COUNTS_LOG']
    omit('Set TOAST_VERB_COUNTS_LOG to the test server counts file') unless log_path

    obj = nil
    begin
      run_test_as('wizard') do
        obj = create(:nothing)
        add_verb(obj, [player, 'rxd', 'counts_reset_probe'], ['this', 'none', 'this'])
        assert_equal [], set_verb_code(obj, 'counts_reset_probe', ['return 7;'])
        3.times { assert_equal 7, call(obj, 'counts_reset_probe') }

        assert_equal 0, simplify(command('; return clear_verb_counts();'))
        assert_equal({}, latest_counts(log_path), 'Reset must publish an empty snapshot immediately')

        assert_equal 7, call(obj, 'counts_reset_probe')
        # Rejected calls must not reset the newly recorded invocation.
        assert_equal E_ARGS, simplify(command('; return clear_verb_counts(1);'))
      end
      run_test_as('programmer') do
        assert_equal E_PERM, simplify(command('; return clear_verb_counts();'))
      end

      key = "#{obj}:counts_reset_probe"
      deadline = Process.clock_gettime(Process::CLOCK_MONOTONIC) + 35
      counts = latest_counts(log_path)
      until counts.key?(key) || Process.clock_gettime(Process::CLOCK_MONOTONIC) >= deadline
        sleep 0.1
        counts = latest_counts(log_path)
      end
      assert_equal 1, counts[key], 'Next periodic snapshot must exclude the three pre-reset calls'
    ensure
      run_test_as('wizard') { recycle(obj) } if obj
    end
  end

  private

  def latest_counts(path)
    # Ignore a final line still being written by the server.
    lines = File.read(path).lines.select { |line| line.end_with?("\n") }
    JSON.parse(lines.last).fetch('counts')
  end

end
