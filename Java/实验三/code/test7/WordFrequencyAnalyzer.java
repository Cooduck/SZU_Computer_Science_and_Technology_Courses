package test7;

import java.util.*;
import java.util.stream.Collectors;

public class WordFrequencyAnalyzer {
    public static void main(String[] args) {
        // 将网页内容复制到字符串变量 `content` 中
        String content = """
            About
            Shenzhen University (SZU) is committed to excellence in teaching, research and social service. 
            Sticking to the motto of “self-reliance, self-discipline, self-improvement”, the University is 
            dedicated to serving the Shenzhen Special Economic Zone (SEZ), demonstrating China’s reform and 
            opening up and pioneering change in higher education.
			
			SZU, which is based in Shenzhen, China’s first Special Economic Zone and a key city in the 
			Guangdong-Hong-Kong-Macau Greater Bay Area, is distinctively known as an Experimental University 
			in higher education with its reforms in the sector acknowledged in Mainland China.
			
			Established in 1983, SZU received support from top Chinese universities including 
			Peking University, Tsinghua University and Renmin University of China in the founding of new 
			schools. In the past decades, the University has undergone rapid growth and has become a 
			comprehensive university with complete disciplines, top-ranked academic and research institutes 
			and awe-inspiring faculty. SZU faculty members are engaged with teaching and research for the 
			betterment of society. They are devoted to seeking solutions to pressing global challenges 
			and promoting innovation.
			
			SZU offers a wide array of undergraduate and graduate programs and provides students with 
			an interdisciplinary and inclusive multicultural learning environment. Students in SZU enjoy 
			the plenty resources and facilities of both the SEZ and the University, pursue academic 
			excellence and discover new interests and opportunities in a fast-changing era.
			
			SZU is an integral part of the SEZ, a thriving technology and innovation hub. With two campuses 
			in Yuehai and Lihu, the University vigorously conducts leading researches in various fields 
			and collaborates with high-tech enterprises in the community for technology transfer. 
			SZU strives to provide a high-quality and effective education and develop in each SZU member 
			the ability and passion to innovate and contribute to social progress and development, 
			and encourages talented young people to start entrepreneurship in SZU. Our alumni including 
			Tencent have founded dozens of innovative companies with significant influence.
			
			SZU is accelerating its pace toward internationalization, providing a variety of global learning 
			opportunities. The University has established partnerships with numbers of overseas universities 
			to offer exceptional exchange programs, joint degree programs, research collaborations, 
			and a variety of other forms of collaborations with international partners. Students 
			from all over the world are welcomed in SZU. In the city noted for its urban vitality 
			and natural beauty, students can explore the most attractive parts of China, pursue their 
			passion and develop their interests, perspectives and abilities.
			""";

        // 创建一个 HashMap 用于存储每个单词及其出现的次数
        Map<String, Integer> wordCountMap = new HashMap<>();

        // 使用正则表达式分割字符串，将文本按非字母字符分割为单词，并转换为小写
        String[] words = content.toLowerCase().split("[^a-zA-Z]+");
        for (String word : words) {
            if (!word.isEmpty()) { // 过滤掉空字符串
                wordCountMap.put(word, wordCountMap.getOrDefault(word, 0) + 1); // 累加每个单词的出现次数
            }
        }

        // 将 wordCountMap 中的条目转换为列表，以便排序
        List<Map.Entry<String, Integer>> sortedWordList = 
        		new ArrayList<>(wordCountMap.entrySet());

        // 按照单词出现次数从高到低排序
        Collections.sort(sortedWordList, new Comparator<Map.Entry<String, Integer>>() {
            @Override
            public int compare(Map.Entry<String, Integer> o1, Map.Entry<String, Integer> o2) {
            	return o2.getValue().compareTo(o1.getValue());
            }
        });

        // 打印出现次数最多的50个单词，每行显示10个单词及其次数
        System.out.println("出现次数最多的50个单词：");
        for (int i = 0; i < sortedWordList.size() && i < 50; i++) {
            Map.Entry<String, Integer> entry = sortedWordList.get(i);
            System.out.print(entry.getKey() + ":" + entry.getValue() + " ");
            if ((i + 1) % 10 == 0) {
                System.out.println();  // 每10个单词换行
            }
        }
    }
}
