package cn.edu;
import java.util.*;

public class SZU {
	private LinkedList<College> colleges;
	
	public SZU() {
		colleges = new LinkedList<College>();
	}
	
	public void addCollege(College c) {
		colleges.add(c);
	}
	
	public void showAllColleges() {
		System.out.print("All colleges: ");
		for(College college : colleges) {
			System.out.print(college.getName() + " ");
		}
		System.out.print("\n");
	}
	
	public int getCollegeNum() {
		return colleges.size();
	}
}
