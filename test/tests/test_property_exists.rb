require 'test_helper'

class TestPropertyExists < Test::Unit::TestCase
  def property_exists(object, name, direct = nil)
    args = [obj_ref(object), value_ref(name)]
    args << value_ref(direct) unless direct.nil?
    evaluate("property_exists(#{args.join(', ')})")
  end

  def test_builtin_and_missing_properties
    run_test_as('programmer') do
      o = create(NOTHING)
      assert_equal 1, property_exists(o, 'name')
      assert_equal 1, property_exists(o, 'name', 0)
      assert_equal 0, property_exists(o, 'name', 1)
      assert_equal 0, property_exists(o, 'missing')
      assert_equal 0, property_exists(o, 'missing', 1)
    end
  end

  def test_direct_and_inherited_properties_do_not_depend_on_value
    run_test_as('programmer') do
      parent = create(NOTHING)
      add_property(parent, 'example', 0, [player, 'r'])
      child = create(parent)
      [0, '', [], [1, 2], 'value'].each do |value|
        set(parent, 'example', value)
        assert_equal 1, property_exists(parent, 'example')
        assert_equal 1, property_exists(parent, 'example', 1)
        assert_equal 1, property_exists(parent, 'example', -1)
        assert_equal 1, property_exists(child, 'example')
        assert_equal 1, property_exists(child, 'example', 0)
        assert_equal 0, property_exists(child, 'example', 1)
      end
    end
  end

  def test_clear_values_through_several_ancestors
    run_test_as('programmer') do
      root = create(NOTHING)
      add_property(root, 'example', 0, [player, 'r'])
      middle = create(root)
      child = create(middle)
      clear_property(middle, 'example')
      clear_property(child, 'example')
      assert_equal 1, is_clear_property(child, 'example')
      assert_equal 1, property_exists(child, 'example')
      assert_equal 0, property_exists(child, 'example', 1)
      assert_equal 1, property_exists(root, 'example', 1)
    end
  end

  def test_anonymous_objects
    run_test_as('programmer') do
      result = simplify(command('; p = create($nothing); add_property(p, "example", 0, {player, "r"}); a = create(p, 1); clear_property(a, "example"); return {property_exists(a, "example"), property_exists(a, "example", 1), property_exists(a, "name"), property_exists(a, "name", 1), property_exists(a, "missing"), is_clear_property(a, "example")};'))
      assert_equal [1, 0, 1, 0, 0, 1], result
    end
  end

  def test_multiple_inheritance_finds_properties_from_both_parents
    run_test_as('programmer') do
      left = create(NOTHING)
      right = create(NOTHING)
      add_property(left, 'left_prop', 0, [player, 'r'])
      add_property(right, 'right_prop', '', [player, 'r'])
      child = create(NOTHING)
      chparents(child, [left, right])
      assert_equal 1, property_exists(child, 'left_prop')
      assert_equal 1, property_exists(child, 'right_prop')
      assert_equal 0, property_exists(child, 'left_prop', 1)
      assert_equal 0, property_exists(child, 'right_prop', 1)
    end
  end

  def test_temporary_anonymous_argument_remains_alive_during_direct_check
    run_test_as('programmer') do
      result = simplify(command('; p = create($nothing); add_property(p, "example", 0, {player, "r"}); return {property_exists(create(p, 1), "example"), property_exists(create(p, 1), "example", 1), property_exists(create(p, 1), "name", 1)};'))
      assert_equal [1, 0, 0], result
    end
  end

  def test_property_name_matching_is_case_insensitive
    run_test_as('programmer') do
      o = create(NOTHING)
      add_property(o, 'Example', 0, [player, 'r'])
      assert_equal 1, property_exists(o, 'EXAMPLE')
      assert_equal 1, property_exists(o, 'example', 1)
      assert_equal 1, property_exists(o, 'NAME')
    end
  end

  def test_errors_for_invalid_objects_types_and_argument_counts
    run_test_as('programmer') do
      assert_equal E_INVARG, property_exists(NOTHING, 'name')
      o = create(NOTHING)
      recycle(o)
      assert_equal E_INVARG, property_exists(o, 'name')
      assert_equal E_INVARG, property_exists(o, 'name', 1)
      assert_equal E_TYPE, evaluate('property_exists(0, "name")')
      assert_equal E_TYPE, evaluate('property_exists(player, 0)')
      assert_equal E_TYPE, evaluate('property_exists(player, "name", "yes")')
      assert_equal E_ARGS, evaluate('property_exists()')
      assert_equal E_ARGS, evaluate('property_exists(player, "name", 0, 0)')
    end
  end

  def test_unreadable_custom_properties_raise_even_with_direct_only
    parent = child = nil
    run_test_as('wizard') do
      parent = create(NOTHING)
      set(parent, 'f', 1)
      add_property(parent, 'private_prop', 0, [player, ''])
      child = create(parent)
      assert_equal 1, property_exists(parent, 'private_prop', 1)
    end
    run_test_as('programmer') do
      assert_equal E_PERM, property_exists(parent, 'private_prop')
      assert_equal E_PERM, property_exists(parent, 'private_prop', 1)
      assert_equal E_PERM, property_exists(child, 'private_prop')
      assert_equal E_PERM, property_exists(child, 'private_prop', 1)
      assert_equal 0, property_exists(child, 'missing')
    end
  end

  def test_lookup_reflects_add_delete_and_reparent_operations
    run_test_as('programmer') do
      root = create(NOTHING)
      child = create(root)
      assert_equal 0, property_exists(child, 'example')
      add_property(root, 'example', 0, [player, 'r'])
      assert_equal 1, property_exists(child, 'example')
      delete_property(root, 'example')
      assert_equal 0, property_exists(child, 'example')
      other = create(NOTHING)
      add_property(other, 'example', 0, [player, 'r'])
      chparent(child, other)
      assert_equal 1, property_exists(child, 'example')
      assert_equal 0, property_exists(child, 'example', 1)
    end
  end
end
