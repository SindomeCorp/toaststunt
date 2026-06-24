require 'test_helper'

class TestSql < Test::Unit::TestCase

  NO_HANDLE = 'No connection handle value by that ID.'

  def test_that_sql_functions_require_wizperms
    skip_unless_sql_available

    run_test_as('programmer') do
      assert_equal E_PERM, simplify(command(%Q|; return sql_connections();|))
      assert_equal E_PERM, simplify(command(%Q|; return sql_open("nosuchdb://localhost/db");|))
      assert_equal E_PERM, simplify(command(%Q|; return sql_query(9999, "select 1");|))
      assert_equal E_PERM, simplify(command(%Q|; return sql_info(9999);|))
      assert_equal E_PERM, simplify(command(%Q|; return sql_close(9999);|))
    end
  end

  def test_that_sql_connections_is_empty_without_open_pools
    skip_unless_sql_available

    run_test_as('wizard') do
      assert_equal({}, simplify(command(%Q|; return sql_connections();|)))
    end
  end

  def test_that_invalid_sql_handles_return_a_plain_error_message
    skip_unless_sql_available

    run_test_as('wizard') do
      assert_equal NO_HANDLE, simplify(command(%Q|; return sql_query(9999, "select 1");|))
      assert_equal NO_HANDLE, simplify(command(%Q|; return sql_info(9999);|))
      assert_equal NO_HANDLE, simplify(command(%Q|; return sql_close(9999);|))
    end
  end

  def test_that_sql_open_rejects_invalid_uris_without_connecting
    skip_unless_sql_available

    run_test_as('wizard') do
      assert_equal E_INVARG, simplify(command(%Q|; return sql_open("not-a-uri");|))
      assert_equal E_INVARG, simplify(command(%Q|; return sql_open("postgresql://");|))
      assert_equal E_INVARG, simplify(command(%Q|; return sql_open("nosuchdb://localhost/db");|))
      assert_equal({}, simplify(command(%Q|; return sql_connections();|)))
    end
  end

  private

  def skip_unless_sql_available
    run_test_as('wizard') do
      omit('SQL builtins are not registered in this build') unless has_function?('sql_connections')
    end
  end

end
